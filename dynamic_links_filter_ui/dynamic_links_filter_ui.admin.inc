<?php

/**
 * @file
 * Administrative page callbacks for the dynamic_links_filter_ui module.
 */

/**
 * Page callback for dynamic links configuration page.
 */
function dynamic_links_filter_ui_admin_links($form, &$form_state) {

  $sql = array('conditions' => array(), 'args' => array(), 'limit' => 50);

  $form['dlfids'] = dynamic_links_filter_ui_list_dynamic_links($sql, TRUE);
  $form['pager'] = array(
    '#type' => 'markup',
    '#markup' => theme('pager', array()),
  );

  return $form;
}

/**
 * Render a list of links for the dynamic links configuration page.
 *
 * @param array $sql
 *   The starting sql conditions and options.
 * @param bool $tableselect
 *   Tableselect.
 *
 * @return array
 *   Render array.
 *
 * @throws Exception
 */
function dynamic_links_filter_ui_list_dynamic_links(array $sql = array(), $tableselect = FALSE) {

  $header = array(
    'text' => array('data' => t('Text'), 'field' => 'text', 'sort' => 'asc'),
    'url' => array('data' => t('Link URL'), 'field' => 'url'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('dynamic_links_filter', 'dlf')
    ->fields('dlf')
    ->addTag('translatable')
    ->orderBy('dlf.text', 'ASC');

  if ($sql['limit']) {
    $query = $query->extend('PagerDefault')->limit($sql['limit']);;
  }

  $result = $query->execute();

  $links = array();
  foreach ($result as $link) {
    $links[$link->dlfid] = $link;
  }

  $destination = drupal_get_destination();
  $rows = array();

  foreach ($links as $dlfid => $link) {
    $row = array();
    if (isset($header['text'])) {
      $row['text'] = htmlentities($link->text);
    }
    if (isset($header['url'])) {
      $row['url'] = _dynamic_links_filter_l($link->url, $link->url);
    }
    if (isset($header['operations'])) {
      $operations = array();
      $operations['edit'] = array(
        'title' => t('Edit'),
        'href' => 'admin/config/content/dynamic-links-filter/edit/' . $dlfid,
        'query' => $destination,
      );
      $operations['delete'] = array(
        'title' => t('Delete'),
        'href' => 'admin/config/content/dynamic-links-filter/delete/' . $dlfid,
        'query' => $destination,
      );
      $link_options = array(
        'links' => $operations,
        'attributes' => array('class' => 'links inline'),
      );
      $row['operations'] = theme('links', $link_options);
    }
    $rows[(string) $dlfid] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('No dynamic links available.'),
        'colspan' => count($header),
        'class' => 'empty',
      ),
    );
  }
  return array(
    '#type' => 'markup',
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );
}

/**
 * Page callback for edit form.
 *
 * @param array $form
 *   Current form.
 * @param array $form_state
 *   Current form state.
 * @param array $record
 *   The record to be edited.
 *
 * @return array
 *   The edit form.
 */
function dynamic_links_filter_ui_edit_form(array $form, array &$form_state, array $record = array()) {
  // Merge default values.
  $record += array(
    'dlfid' => NULL,
    'text' => '',
    'url' => '',
  );

  $form['dlfid'] = array(
    '#type' => 'value',
    '#value' => $record['dlfid'],
  );

  $form['text'] = array(
    '#type' => 'textfield',
    '#title' => t('Text'),
    '#description' => t("Enter text"),
    '#maxlength' => 560,
    '#default_value' => $record['text'],
    '#required' => TRUE,
  );
  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Link To'),
    '#maxlength' => 560,
    '#default_value' => $record['url'],
    '#required' => TRUE,
    '#description' => t('Enter an internal Drupal path, path alias, or complete external URL (like http://example.com/) to link to. Use %front to redirect to the front page.', array('%front' => '<front>')),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['cancel'] = array(
    '#value' => l(t('Cancel'), isset($_REQUEST['destination']) ? $_REQUEST['destination'] : 'admin/config/content/dynamic-links-filter'),
  );

  return $form;
}

/**
 * Validation callback for the add/edit form.
 *
 * @param array $form
 *   The add/edit form.
 * @param array $form_state
 *   The add/edit form state.
 */
function dynamic_links_filter_ui_edit_form_validate(array $form, array &$form_state) {
  $record = dynamic_links_filter_ui_load_by_text($form_state['values']['text']);
  if ($record && $form_state['values']['dlfid'] != $record['dlfid']) {
    form_set_error('text', t('The text %text is already being linked. Do you want to <a href="@edit-page">edit the existing link</a>?', array('%text' => $record['text'], '@edit-page' => url('admin/config/content/dynamic-links-filter/edit/' . $record['dlfid']))));
  }
  if (!valid_url($form_state['values']['url']) && !valid_url($form_state['values']['url'], TRUE) && $form_state['values']['url'] != '<front>') {
    form_set_error('url', t('The link url does not appear valid.'));
  }
}

/**
 * Submit callback for the add/edit form.
 *
 * @param array $form
 *   The add/edit form.
 * @param array $form_state
 *   The add/edit form state.
 */
function dynamic_links_filter_ui_edit_form_submit(array $form, array &$form_state) {
  $record = $form_state['values'];
  $record['text'] = strtolower(trim(check_plain($record['text'])));
  $record['url'] = trim(drupal_strip_dangerous_protocols($record['url']), '\/?');
  if (empty($record['dlfid'])) {
    $result = drupal_write_record('dynamic_links_filter', $record);
  }
  else {
    $result = drupal_write_record('dynamic_links_filter', $record, array('dlfid'));
  }
  drupal_set_message($result ? t('The dynamic link has been saved.') : t('An unknown error occurred'));
  if ($result) {
    _dynamic_links_filter_clear_cache_by_filter('dynamic_links_filter');
  }
  $form_state['redirect'] = 'admin/config/content/dynamic-links-filter';
}

/**
 * Page callback for Delete confirmation form.
 *
 * @param array $form
 *   The current form.
 * @param array $form_state
 *   The current form state.
 * @param array $record
 *   The record to delete.
 *
 * @return array
 *   The delete confirmation form.
 */
function dynamic_links_filter_ui_delete_form(array $form, array &$form_state, array $record) {
  $form['record'] = array(
    '#type' => 'value',
    '#value' => $record,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the text %text to %url?', array('%text' => $record['text'], '%url' => $record['url'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/content/dynamic-links-filter'
  );
}

/**
 * Submit callback for the Delete confirmation form.
 *
 * @param array $form
 *   The delete form.
 * @param array $form_state
 *   The delete form state.
 */
function dynamic_links_filter_ui_delete_form_submit(array $form, array &$form_state) {
  $deleted = db_delete('dynamic_links_filter')
    ->condition('dlfid', $form_state['values']['record']['dlfid'])
    ->execute();
  drupal_set_message($deleted ? t('The dynamic link has been deleted.') : t('An unknown error occurred'));
  if ($deleted) {
    _dynamic_links_filter_clear_cache_by_filter('dynamic_links_filter');
  }
  $form_state['redirect'] = 'admin/config/content/dynamic-links-filter';
}
