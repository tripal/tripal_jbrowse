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

  $settings = tripal_jbrowse_mgmt_get_settings();
  $path = $settings['data_dir'] . '/genus_species/data/';

  $msg = '<p>Register an already existing JBrowse instance to be managed by this module.</p>'
  . '<div class="messages warning">It is expected that the data directory for the existing JBrowse instance is <code>@path</code> where <code>genus_species</code> matches what you select for the organism. If this is not the location of your JBrowse, you should create a symbolic link between the current instance location and the expected one mentioned above.</div>';
  $form['description_of_form']['#markup'] = t($msg, ['@path' => $path]);

  // Remove the file upload.
  unset($form['data']);

  // Change the submit button.
  $form['submit']['#value'] = 'Register Instance';

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

  // Check an instance does not already exist.
  $instances = tripal_jbrowse_mgmt_get_instances(['organism_id' => $organism]);
  if (!empty($instances)) {
    form_set_error(
      'organism',
      'A JBrowse instance for the selected organism already exists. You can edit the instance from the instances page.'
    );
  }

  // Check that the organism does exist.
  $organism = db_select('chado.organism', 'CO')
    ->fields('CO')
    ->condition('organism_id', $organism)
    ->execute()
    ->fetchObject();

  if (empty($organism)) {
    form_set_error('organism', 'Invalid organism selected ' . $organism);
  }

  // Check that the directory we assume contains the instance does exist.
  $title = tripal_jbrowse_mgmt_construct_organism_name($organism);
  $settings = tripal_jbrowse_mgmt_get_settings();
  $path = $settings['data_dir'];
  $path .= '/' . tripal_jbrowse_mgmt_make_slug($title);
  $path .= '/data/trackList.json';
  if (!file_exists($path)) {
    form_set_error('organism', "We expect there is already an existing instane at the following path: $path");
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
    drupal_set_message('Instance registered successfully!');
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
