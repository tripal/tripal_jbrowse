<?php
/**
 * @file
 * Builds the public jbrowse listing.
 */

/**
 * Builds the public jbrowse listing.
 */
function tripal_jbrowse_page_listing_page() {
  $settings = tripal_jbrowse_mgmt_get_settings();

  // Retrieve all the instances...
  $instances = tripal_jbrowse_mgmt_get_instances();

  drupal_add_css(drupal_get_path('module', 'tripal_jbrowse_page') . '/theme/tripal_jbrowse_page.css');

  // Add the URL for each to link to.
  foreach($instances as $k => $instance) {
    $instances[$k]->url = url('jbrowse/'.$instance->organism->genus . '/' . $instance->organism->species, ['absolute' => TRUE]);
  }

  // Use the template to render the page.
  return theme('jbrowse_instance_public_listing', ['instances' => $instances]);
}
