<?php

/**
 * @file
 * Administrative page callbacks for the dynamic_links_filter_ui module.
 */

/**
 * Page callback for dynamic links configuration page.
 */
function dynamic_links_filter_ui_admin_links($form, &$form_state) {

  // Starting sql conditions for the dynamic links list select query.
  $sql = array('conditions' => array(), 'args' => array(), 'limit' => 50);

  // Adds a list of dynamic links with edit/delete links and pager to the form.
  $form['dlfids'] = dynamic_links_filter_ui_list_dynamic_links($sql);
  $form['pager'] = array(
    '#type' => 'markup',
    '#markup' => theme('pager', array()),
  );

  return $form;
}

/**
 * Returns a render array of links for the dynamic links configuration page.
 *
 * The render array is an HTML table theme.
 *
 * @param array $sql
 *   The starting sql conditions and options.
 *
 * @return array
 *   Render array.
 *
 * @throws Exception
 */
function dynamic_links_filter_ui_list_dynamic_links(array $sql = array()) {

  // Define the table header.
  $header = array(
    'text' => array('data' => t('Text'), 'field' => 'text', 'sort' => 'asc'),
    'url' => array('data' => t('Link URL'), 'field' => 'url'),
    'operations' => array('data' => t('Operations')),
  );

  // Get the current mappings from the database, ordered alphabetically.
  $query = db_select('dynamic_links_filter', 'dlf')
    ->fields('dlf')
    ->addTag('translatable')
    ->orderBy('dlf.text', 'ASC');

  // Add a pager limited by the specified limit.
  if ($sql['limit']) {
    $query = $query->extend('PagerDefault')->limit($sql['limit']);;
  }

  // Get the results as an array keyed by the table id.
  $links = $query->execute()->fetchAllAssoc('dlfid');

  // Set the return page.
  $destination = drupal_get_destination();
  $rows = array();

  // Build the table rows.
  foreach ($links as $dlfid => $link) {
    $row = array();
    // Fill the columns for each header column.
    if (isset($header['text'])) {
      // Get plain text for the keyword text.
      $row['text'] = htmlentities($link->text);
    }
    if (isset($header['url'])) {
      // Create a link for the url column.
      $row['url'] = _dynamic_links_filter_l($link->url, $link->url);
    }
    if (isset($header['operations'])) {
      // Create the edit/delete links in the operations column.
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
      // Theme the operations links as inline theme_link.
      $link_options = array(
        'links' => $operations,
        'attributes' => array('class' => 'links inline'),
      );
      $row['operations'] = theme('links', $link_options);
    }
    // Add this row to the list of rows keyed by id.
    // The id needs to be a string so that it's not an array index.
    $rows[(string) $dlfid] = $row;
  }

  // Now build the theme_table render array.
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' =>t('No dynamic links available.'),
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

  // Add fields for the dynamic link edit form.
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
  // Get the database record for the keyword text entered.
  $record = dynamic_links_filter_ui_load_by_text($form_state['values']['text']);

  // Do not add another record if the keyword text already exists.
  if ($record && $form_state['values']['dlfid'] != $record['dlfid']) {
    form_set_error('text', t('The text %text is already being linked. Do you want to <a href="@edit-page">edit the existing link</a>?', array('%text' => $record['text'], '@edit-page' => url('admin/config/content/dynamic-links-filter/edit/' . $record['dlfid']))));
  }
  // Ensure a valid absolute or relative url.
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
  // Get the record to add/edit.
  $record = $form_state['values'];

  // Cleanup the input before storing.
  $record['text'] = strtolower(trim(check_plain($record['text'])));
  $record['url'] = trim(drupal_strip_dangerous_protocols($record['url']), '\/?');

  // Write a new record if there is no id.
  if (empty($record['dlfid'])) {
    $result = drupal_write_record('dynamic_links_filter', $record);
  }
  // Otherwise update the record.
  else {
    $result = drupal_write_record('dynamic_links_filter', $record, array('dlfid'));
  }

  // Set a friendly result message.
  drupal_set_message($result ? t('The dynamic link has been saved.') : t('An unknown error occurred'));

  // Reset the filter cache when a mapping has been added/edited.
  if ($result) {
    _dynamic_links_filter_clear_cache_by_filter('dynamic_links_filter');
  }

  // Redirect back to the list.
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
  // Add the record (from the wildcard loader) as a value form field.
  $form['record'] = array(
    '#type' => 'value',
    '#value' => $record,
  );

  // Build and return a confirmation form for when delete button pressed.
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
  // Delete the record given in the value field.
  $deleted = db_delete('dynamic_links_filter')
    ->condition('dlfid', $form_state['values']['record']['dlfid'])
    ->execute();

  // Set a friendly result message.
  drupal_set_message($deleted ? t('The dynamic link has been deleted.') : t('An unknown error occurred'));

  // Clear the filter cache after deleting.
  if ($deleted) {
    _dynamic_links_filter_clear_cache_by_filter('dynamic_links_filter');
  }

  // Go back to the list page.
  $form_state['redirect'] = 'admin/config/content/dynamic-links-filter';
}
