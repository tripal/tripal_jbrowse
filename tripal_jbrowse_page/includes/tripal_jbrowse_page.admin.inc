<?php
/**
 * @file
 * Administration (settings) for this module.
 */

/**
 * Settings Form for page integration.
 */
function tripal_jbrowse_page_settings_form($form, $form_state) {

  $form['general'] = [
    '#type' => 'fieldset',
    '#title' => 'General Page Integration Settings'
  ];

  $form['general']['create_menu_items'] = [
    '#type' => 'checkbox',
    '#title' => 'Create Menu Items',
    '#description' => 'Checking this box ensures that menu items under
      "Navigation" will be created for each JBrowse instance. This only
      applies to JBrowse Instances with a page.',
    '#default_value' => variable_get('trpjbrowse_page_create_menu_items', 1),
  ];

  $form['exclude_fieldset'] = [
    '#type' => 'fieldset',
    '#title' => 'Exclude JBrowse Instances from Page Integration',
    '#description' => 'To exclude a JBrowse instance from page integration (i.e. no embedded page will be shown and it won\'t show up in the listing), check the checkbox below.',
  ];

  // Get all instances as an option to be excluded.
  $instances = tripal_jbrowse_mgmt_get_instances();
  $options = [];
  foreach ($instances as $instance) {
    $key = $instance->id;
    $options[$key] = [
      'genus' => $instance->organism->genus,
      'species' => $instance->organism->species,
    ];
  }
  // Determine the default value.
  $default_exclude = variable_get('trpjbrowse_page_exclude', []);
  if (!is_array($default_exclude)) {
    $default_exclude = unserialize($default_exclude);
  }
  // Now generate the table.
  $form['exclude_fieldset']['exclude'] = [
    '#type' => 'tableselect',
    '#header' => [
      'genus' => t('Genus'),
      'species' => t('Species'),
    ],
    '#options' => $options,
    '#empty' => t('No JBrowse Instances available. Please add one through the "List Instances" page.'),
    '#default_value' => $default_exclude,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save Settings',
  ];

  return $form;
}

/**
 * Settings Form for page integration.
 */
function tripal_jbrowse_page_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];

  // General Settings.
  variable_set(
    'trpjbrowse_page_create_menu_items',
    $values['create_menu_items']
  );

  // Exclude Instances.
  variable_set(
    'trpjbrowse_page_exclude',
    serialize($values['exclude'])
  );

  // Ensure the menu is rebuilt.
  variable_set('menu_rebuild_needed', TRUE);

}
