<?php

/**
 * @file
 * Views hook implementations for dynamic links filter.
 */

/**
 * Implements hook_views_data().
 */
function dynamic_links_filter_ui_views_data() {

  $data = array();

  $data['dynamic_links_filter']['table']['group'] = t('Dynamic Links Filter');

  $data['dynamic_links_filter']['table']['base'] = array(
    'title' => t('Dynamic Links Filter'),
    'help' => t('Keyword to link data.'),
  );

  // The ID field.
  $data['dynamic_links_filter']['dlfid'] = array(
    'title' => t('ID'),
    'help' => t('The dynamic_links_filter ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // The text field.
  $data['dynamic_links_filter']['text'] = array(
    'title' => t('Text'),
    'help' => t('The text to convert.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // The Url field.
  $data['dynamic_links_filter']['url'] = array(
    'title' => t('URL'),
    'help' => t('The url of the resulting link.'),
    'field' => array(
      'handler' => 'views_handler_field_url',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  return $data;

}
