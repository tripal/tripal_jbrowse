<?php

function tripal_jbrowse_mgmt_add_form($form, &$form_state) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  // Ensure the module is configured.
  if (empty(array_filter($settings))) {
    $form['incomplete'] = [
      '#type' => 'item',
      '#prefix' => '<div class="messages error">',
      '#markup' => t(
        'You have not configured the module yet. Please visit the
            <a href="@url">settings page</a> and submit the form before continuing.',
        ['@url' => url('admin/tripal/extension/tripal_jbrowse/management/configure')]
      ),
      '#suffix' => '</div>',
    ];

    return $form;
  }

  // Check to see if this is an edit form.
  $edit_form = FALSE;
  if (isset($form_state['build_info']['args'][0]) AND is_numeric($form_state['build_info']['args'][0])) {
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
  }

  $organisms = tripal_jbrowse_mgmt_get_organisms_list();
  $mapped_organisms = [];
  foreach ($organisms as $organism) {
    $mapped_organisms[$organism->organism_id] = tripal_jbrowse_mgmt_construct_organism_name(
      $organism
    );
  }

  $form_description = 'Create a new JBrowse instance for a given organism. Submitting this form
  creates all the necessary files for a new JBrowse instance.';
  if ($edit_form) {
    $form_description = 'Edit details regarding the current JBrowse instance.';
  }
  $form['description_of_form'] = [
    '#type' => 'item',
    '#markup' => t($form_description),
  ];

  $form['organism'] = [
    '#title' => t('Organism'),
    '#description' => t('Select the organism'),
    '#type' => 'select',
    '#options' => $mapped_organisms,
    '#required' => TRUE,
    '#default_value' => ($edit_form) ? $instance->organism_id : NULL,
  ];

  $form['description'] = [
    '#title' => t('Description'),
    '#description' => t('Optional description for the instance.'),
    '#type' => 'textarea',
    '#default_value' => ($edit_form) ? $instance->description : NULL,
  ];

  $form['data'] = [
    '#type' => 'fieldset',
    '#title' => t('Reference Sequence File'),
    '#collabsible' => FALSE,
  ];

  $form['data']['data_desc'] = [
    '#type' => 'item',
    '#markup' => t(
      'You may either upload a file below or provide
       the path to the reference sequence fasta file.'
    ),
  ];

  $form['data']['ref_seq_file'] = [
    '#type' => 'file',
    '#title' => t('Reference Sequence FASTA File'),
    '#disabled' => ($edit_form) ? TRUE : FALSE,
  ];

  $form['data']['ref_seq_path'] = [
    '#type' => 'textfield',
    '#title' => t('OR Path to File on Server'),
    '#description' => t(
      'This path will be ignored if a file is provided above. Ex: sites/default/files/file.fasta or /data/file.fasta'
    ),
    '#disabled' => ($edit_form) ? TRUE : FALSE,
  ];

  $button = 'Create New Instance';
  if ($edit_form) {
    $button = 'Save Changes';
  }
  $form['submit'] = [
    '#type' => 'submit',
    '#value' => $button,
  ];

  return $form;
}

/**
 * Validate the form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_jbrowse_mgmt_add_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $organism = isset($values['organism']) ? $values['organism'] : NULL;

  // Check if this is an add or edit form.
  $edit_form = FALSE;
  if (isset($form_state['build_info']['args'][0]) AND is_numeric($form_state['build_info']['args'][0])) {
    $instance_id = $form_state['build_info']['args'][0];
    $edit_form = TRUE;
  }

  if (!$edit_form) {
    $file = $_FILES['files']['tmp_name']['ref_seq_file'];
    $local_file = isset($values['ref_seq_path']) ? $values['ref_seq_path'] : NULL;

    if (empty($file) && empty($local_file)) {
      form_set_error(
        'ref_seq_file',
        'Please provide a local file path or upload a new file.'
      );
    }
    elseif (empty($file) && !empty($local_file)) {
      if (!file_exists($local_file)) {
        form_set_error('ref_seq_path', 'The file path provided does not exist.');
      }
    }
    else {
      $uploaded = tripal_jbrowse_mgmt_upload_file('ref_seq_file');
      if (!$uploaded) {
        form_set_error('ref_seq_file', 'Unable to upload file');
      }
      else {
        $uploaded = tripal_jbrowse_mgmt_move_file($uploaded);
        $form_state['values']['uploaded_file'] = $uploaded;
      }
    }
  }

  $instances = tripal_jbrowse_mgmt_get_instances(['organism_id' => $organism]);
  if (!empty($instances)) {
    if (!$edit_form OR ($edit_form AND $instances[0]->id != $instance_id)) {
      form_set_error(
        'organism',
        'A JBrowse instance for the selected organism already exists. You can edit the instance from the instances page.'
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
function tripal_jbrowse_mgmt_add_form_submit($form, &$form_state) {
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

  if ($edit_form) {

    $organism = db_select('chado.organism', 'CO')
      ->fields('CO')
      ->condition('organism_id', $organism_id)
      ->execute()
      ->fetchObject();

    $title = tripal_jbrowse_mgmt_construct_organism_name($organism);

    $success = db_update('tripal_jbrowse_mgmt_instances')
      ->fields([
        'organism_id' => $organism_id,
        'title' => $title,
        'description' => $description,
      ])
      ->condition('id', $instance_id)
      ->execute();

    if ($success) {
      drupal_set_message("Successfully updated $title JBrowse instance.");
      $form_state['redirect'] = 'admin/tripal/extension/tripal_jbrowse/management/instances';
    }
    else {
      drupal_set_message('Failed to update the current instance!', 'error');
    }
  }
  else {
    if (empty($values['uploaded_file'])) {
      $file = $values['ref_seq_path'];
    }
    else {
      $file = $values['uploaded_file'];
    }

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
        'file' => $file,
      ]
    );

    if ($instance_id) {
      drupal_set_message('Instance created successfully!');
      $name = 'Create JBrowse instance for ';
      $name .= tripal_jbrowse_mgmt_construct_organism_name($organism);

      tripal_add_job(
        $name,
        'tripal_jbrowse_mgmt',
        'tripal_jbrowse_mgmt_create_instance_files',
        [$instance_id],
        $user->uid
      );
      $form_state['redirect'] = "admin/tripal/extension/tripal_jbrowse/management/instances/$instance_id";
      return $form;
    }
    drupal_set_message('Failed to create instance!', 'error');
  }
}
