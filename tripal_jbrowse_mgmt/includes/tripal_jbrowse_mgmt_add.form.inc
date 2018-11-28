<?php

function tripal_jbrowse_mgmt_add_form($form, &$form_state) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  if (empty(array_values($settings))) {
    $form['incomplete'] = [
      '#type' => 'item',
      '#prefix' => '<div style="color: red;">',
      '#markup' => t(
        'You have not configured the module yet. Please visit the 
                      settings page and submit the form before continuing.'
      ),
      '#suffix' => '</div>',
    ];

    return;
  }

  $organisms = tripal_jbrowse_mgmt_get_organisms_list();
  $mapped_organisms = [];
  foreach ($organisms as $organism) {
    $mapped_organisms[$organism->organism_id] = tripal_jbrowse_mgmt_construct_organism_name(
      $organism
    );
  }

  $form['description_of_form'] = [
    '#type' => 'item',
    '#markup' => t(
      'Create a new JBrowse instance for a given organism. Submitting this form 
      creates all the necessary files for a new JBrowse instance.'
    ),
  ];

  $form['organism'] = [
    '#title' => t('Organism'),
    '#description' => t('Select the organism'),
    '#type' => 'select',
    '#options' => $mapped_organisms,
    '#required' => TRUE,
  ];

  $form['description'] = [
    '#title' => t('Description'),
    '#description' => t('Optional description for the instance.'),
    '#type' => 'textarea',
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
  ];

  $form['data']['ref_seq_path'] = [
    '#type' => 'textfield',
    '#title' => t('OR Path to File on Server'),
    '#description' => t(
      'This path will be ignored if a file is provided above. Ex: sites/default/files/file.fasta or /data/file.fasta'
    ),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Create New Instance',
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
function tripal_jbrowse_mgmt_add_form_submit($form, &$form_state) {
  global $user;

  $values = $form_state['values'];
  $organism_id = $values['organism'];
  $description = isset($values['description']) ? $values['description'] : '';

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
    drupal_goto("admin/tripal_jbrowse_mgmt/instances/$instance_id");
    return $form;
  }

  drupal_set_message('Failed to create instance!');
}
