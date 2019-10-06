<?php
/**
 * @file
 * Builds the Tripal JBrowse page.
 */

/**
 * Redirect to the JBrowse Instance.
 */
function tripal_jbrowse_page_page($genus, $species) {

  $instance = tripal_jbrowse_page_get_instance_id([
    'genus' => $genus,
    'species' => $species
  ],
  ['load_instance' => TRUE]);

  $query_params = tripal_jbrowse_mgmt_build_http_query($instance);
  $settings = tripal_jbrowse_mgmt_get_settings();

  $url = url($settings['link'],['query' => $query_params]);

  if (FALSE) {
    drupal_goto($url, array('external' => TRUE));
  }
  else {
    drupal_add_css(drupal_get_path('module', 'tripal_jbrowse_page') . '/theme/tripal_jbrowse_page.css');
    return theme('jbrowse_instance_embedded_page', ['url' => $url]);
  }

  return '';
}
