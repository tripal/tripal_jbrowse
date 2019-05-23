<?php

/**
 * @param $form
 * @param $form_state
 * @param int $instance_id The Instance ID
 */
function tripal_jbrowse_mgmt_delete_instance_form(
  $form,
  &$form_state,
  $instance_id
) {
  $instance = tripal_jbrowse_mgmt_get_instance($instance_id);
  if (empty($instance)) {
    drupal_not_found();
    return [];
  }

  $form['confirm_message'] = [
    '#type' => 'item',
    '#markup' => 'Are you sure you want to permanently delete ' . $instance->title . ' JBrowse instance?',
  ];

  $form['instance_id'] = [
    '#type' => 'hidden',
    '#value' => $instance_id,
  ];

  $form['delete'] = [
    '#type' => 'submit',
    '#value' => 'Delete Instance',
  ];

  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l(
      'Cancel',
      'admin/tripal/extension/tripal_jbrowse/management/instances'
    ),
  ];

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function tripal_jbrowse_mgmt_delete_instance_form_validate($form, &$form_state) {
  $instance_id = $form_state['values']['instance_id'] ?? NULL;
  if (is_null($instance_id)) {
    form_set_error('instance_id', 'Please provide a valid instance');
  }

  $instance = tripal_jbrowse_mgmt_get_instance($instance_id);
  if (empty($instance)) {
    form_set_error('instance_id', 'Please provide a valid instance');
  }
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_delete_instance_form_submit($form, &$form_state) {
  $instance_id = $form_state['values']['instance_id'];
  $instance = tripal_jbrowse_mgmt_get_instance($instance_id);
  tripal_jbrowse_mgmt_delete_instance($instance);

  $settings = tripal_jbrowse_mgmt_get_settings();

  $path = $settings['data_dir'] . '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  drupal_set_message('Instance deleted successfully. A job has been created to delete the instance files. ');

  global $user;
  tripal_add_job('Delete JBrowse Instance Files', 'tripal_jbrowse_mgmt', 'tripal_jbrowse_mgmt_delete_instance_files', [$path], $user->uid);

  drupal_goto('admin/tripal/extension/tripal_jbrowse/management/instance/' . $instance_id);
}
