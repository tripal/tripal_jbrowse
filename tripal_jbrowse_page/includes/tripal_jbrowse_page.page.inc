<?php
/**
 * @file
 * Builds the Tripal JBrowse page.
 */

/**
 * Redirect to the JBrowse Instance.
 */
function tripal_jbrowse_page_page($scientific_name, $instance_id = NULL) {
  if($instance_id){
    $instance = tripal_jbrowse_mgmt_get_instance($instance_id);
  }
  else{
    list($genus, $species) = explode('-', $scientific_name);
    $instance = tripal_jbrowse_page_get_instance_id([
      'genus' => $genus,
      'species' => $species
    ],
    ['load_instance' => TRUE]);
  }

  // Determine Query paramters.
  $query_params = tripal_jbrowse_mgmt_build_http_query($instance);
  $page_q = drupal_get_query_parameters();
  foreach ($page_q as $qkey => $qvalue) {
    $query_params[$qkey] = $qvalue;
  }

  // Build the URL.
  $settings = tripal_jbrowse_mgmt_get_settings();
  $url = url($settings['link'],['query' => $query_params]);

  if (variable_get('trpjbrowse_page_embed', 1)) {
    drupal_add_css(drupal_get_path('module', 'tripal_jbrowse_page') . '/theme/tripal_jbrowse_page.css');
    return theme('jbrowse_instance_embedded_page', ['url' => $url]);
  }
  else {
    drupal_goto($url, array('external' => TRUE));
  }
}
