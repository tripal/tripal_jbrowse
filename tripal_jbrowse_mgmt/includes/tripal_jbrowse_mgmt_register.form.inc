<?php
/**
 * @file
 * Instance Register Form.
 */

/**
 *
 */
function tripal_jbrowse_mgmt_register_form($form, &$form_state) {

  // This is essentiall the add form without create JBrowse functionality.
  // We use a second function here for cleaner validation and submit.
  require_once 'tripal_jbrowse_mgmt_add.form.inc';
  $form = tripal_jbrowse_mgmt_add_form($form, $form_state);

  $form['description_of_form']['#markup'] = t('Register an already existing JBrowse instance to be managed by this module.');

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
function tripal_jbrowse_mgmt_register_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $organism = isset($values['organism']) ? $values['organism'] : NULL;

  $instances = tripal_jbrowse_mgmt_get_instances(['organism_id' => $organism]);
  if (!empty($instances)) {
    form_set_error(
      'organism',
      'A JBrowse instance for the selected organism already exists. You can edit the instance from the instances page.'
    );
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
function tripal_jbrowse_mgmt_register_form_submit($form, &$form_state) {
  global $user;

  $values = $form_state['values'];
  $organism_id = $values['organism'];
  $description = isset($values['description']) ? $values['description'] : '';

  $organism = db_select('chado.organism', 'CO')
    ->fields('CO')
    ->condition('organism_id', $organism_id)
    ->execute()
    ->fetchObject();

  $instance_id = tripal_jbrowse_mgmt_create_instance(
    [
      'organism_id' => $organism_id,
      'title' => tripal_jbrowse_mgmt_construct_organism_name($organism),
      'description' => $description,
      'created_at' => time(),
      'file' => '',
    ]
  );

  if ($instance_id) {
    $instance = tripal_jbrowse_mgmt_get_instance($instance_id);
    $settings = tripal_jbrowse_mgmt_get_settings();
    $slug = tripal_jbrowse_mgmt_make_slug($instance->title);
    if (isset($settings['data_path']) and !empty($settings['data_path'])) {
      $data_path = 'WEB_ROOT/' . $settings['data_path'] . '/' . $slug . '/data';
    }
    else {
      $data_path = 'JBROWSE_ROOT/data/' . $slug . '/data';
    }
    drupal_set_message('Instance registered successfully!');
    drupal_set_message(t('This assumes you have an existing JBrowse set-up at @path. If the link below does not work, perhaps you need to create, rather then register, a new JBrowse Instance.',
      ['@path' => $data_path]), 'warning');
    $form_state['redirect'] = "admin/tripal/extension/tripal_jbrowse/management/instances/$instance_id";
  }
  else {
    drupal_set_message('Failed to create instance!', 'error');
    return;
  }

  // Now save the instance properties.
  tripal_jbrowse_mgmt_save_instance_properties(
    $instance_id,
    $form_state['values']['page']
  );
}
