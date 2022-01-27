<?php
/**
 * @file
 * Instance Edit Form.
 */

/**
 *
 */
function tripal_jbrowse_mgmt_edit_form($form, &$form_state) {

  $instance_id = $form_state['build_info']['args'][0];
  $result = tripal_jbrowse_mgmt_get_instances(['id' => $instance_id]);
  if (!empty($result)) {
    $instance = $result[0];
    $edit_form = TRUE;
  }
  else {
    drupal_set_message('Unable to access the instance you would like to edit.', 'error');
    return $form;
  }

  // This is essentiall the add form with defaults.
  // We use a second function here for cleaner validation and submit.
  require_once 'tripal_jbrowse_mgmt_add.form.inc';
  $form = tripal_jbrowse_mgmt_add_form($form, $form_state);

  $form['description_of_form']['#markup'] = t('Edit details regarding the current JBrowse instance.');

  // Set Default Values.
  $form['organism']['#default_value'] = $instance->organism_id;
  $form['description']['#default_value'] = $instance->description;

  // Sequence Assembly field once set will keep the
  // same value - field is disabled.
  $analysis_name = ($instance->analysis_id) ? $instance->analysis->name : ''; 
  $form['analysis']['#default_value'] = $analysis_name;
  $form['analysis']['#disabled'] = TRUE;

  $form['page']['start-loc']['#default_value'] = tripal_jbrowse_mgmt_get_instance_property($instance_id, 'start-loc');
  $form['page']['start-tracks']['#default_value'] = tripal_jbrowse_mgmt_get_instance_property($instance_id, 'start-tracks');

  // Remove the file upload.
  unset($form['data']);

  // Change the submit button.
  $form['submit']['#value'] = 'Save Changes';

  return $form;
}

/**
 * Validate the form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_jbrowse_mgmt_edit_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $organism = isset($values['organism']) ? $values['organism'] : NULL;

  $instance_id = $form_state['build_info']['args'][0];
  $instances = tripal_jbrowse_mgmt_get_instances(['organism_id' => $organism]);
  
  // Get the current instance.
  $result = tripal_jbrowse_mgmt_get_instances(['id' => $instance_id]);
  if (empty($result)) {
    drupal_set_message('Unable to access the instance you would like to edit.', 'error');
  }
  else {
    $current_instance = $result[0]; 
  }
  
  // When saving modifications, validate organism and analysis
  // to make sure that no instances share the same organism and analysis.
  // When anaysis is not set, ensure the same for instances without analysis.
  $current_instance_analysis_id = ($current_instance->analysis_id)
    ? $current_instance->analysis_id : 0;
  
  foreach($instances as $instance) {
    // Just see if there is other out there so exclude itself.
    if ($instance->id == $instance_id) continue;

    $instance_analysis_id = ($instance->analysis_id) ? $instance->analysis_id : 0;
    if ($instance_analysis_id == $current_instance_analysis_id) {
      // In the pools of instances (same organism), an instance was found
      // matching the orgnism and analysis id (set or not set).
      form_set_error(
        'organism',
        'A JBrowse instance with the same organism and sequence assembly already exits. Please select a different organism.'
      );
    }
  }

  $organism = db_select('chado.organism', 'CO')
    ->fields('CO')
    ->condition('organism_id', $organism)
    ->execute()
    ->fetchObject();

  if (empty($organism)) {
    form_set_error('organism', 'Invalid organism selected ' . $organism);
  }
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_edit_form_submit($form, &$form_state) {
  global $user;

  $values = $form_state['values'];
  $organism_id = $values['organism'];
  $description = isset($values['description']) ? $values['description'] : '';

  // Check if this is an add or edit form.
  $edit_form = FALSE;
  if (isset($form_state['build_info']['args'][0]) AND is_numeric($form_state['build_info']['args'][0])) {
    $instance_id = $form_state['build_info']['args'][0];
    $edit_form = TRUE;
  }

  $organism = db_select('chado.organism', 'CO')
    ->fields('CO')
    ->condition('organism_id', $organism_id)
    ->execute()
    ->fetchObject();

  $title = tripal_jbrowse_mgmt_construct_organism_name($organism);

  $data = [
    'organism_id' => $organism_id,
    'title' => $title,
    'description' => $description,
  ];
  $success = tripal_jbrowse_mgmt_update_instance($instance_id, $data);

  if ($success) {
    drupal_set_message("Successfully updated $title JBrowse instance.");
    $form_state['redirect'] = 'admin/tripal/extension/tripal_jbrowse/management/instances';
  }
  else {
    drupal_set_message('Failed to update the current instance!', 'error');
    return FALSE;
  }

  // Now save the instance properties.
  tripal_jbrowse_mgmt_save_instance_properties(
    $instance_id,
    $form_state['values']['page']
  );
}
