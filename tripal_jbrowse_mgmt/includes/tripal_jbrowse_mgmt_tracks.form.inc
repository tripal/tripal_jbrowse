<?php

/**
 * @file tripal_jbrowse_mgmt_tracks.form.inc
 */

/**
 * Add a track to an instance form.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The form state array.
 * @param int $instance_id
 *   The parent instance ID to add the new track to.
 *
 * @return array
 *   The modified form array.
 */
function tripal_jbrowse_mgmt_add_track_form($form, &$form_state, $instance_id) {
  if (empty(tripal_jbrowse_mgmt_get_instance($instance_id))) {
    drupal_not_found();

    return [];
  }

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Track Label'),
    '#description' => t('This will appear on the sidebar.'),
    '#required' => TRUE,
  ];

  $form['instance_id'] = [
    '#type' => 'hidden',
    '#value' => $instance_id,
  ];

  $form['data'] = [
    '#type' => 'fieldset',
    '#title' => t('Track Files'),
  ];

  $form['data']['track_type'] = [
    '#type' => 'select',
    '#description' => t('See http://gmod.org/wiki/JBrowse_Configuration_Guide#flatfile-to-json.pl for more info.'),
    '#required' => TRUE,
    '#title' => t('Track Type'),
    '#options' => drupal_map_assoc(tripal_jbrowse_mgmt_get_track_types()),
  ];

  $form['data']['file_type'] = [
    '#type' => 'select',
    '#title' => t('File Type'),
    '#options' => drupal_map_assoc(['gff', 'bed', 'gbk', 'vcf', 'bw']),
    '#description' => t('See http://gmod.org/wiki/JBrowse_Configuration_Guide#flatfile-to-json.pl for more info.'),
    '#required' => TRUE,
  ];

  $form['data']['file'] = [
    '#type' => 'file',
    '#title' => t('File'),
  ];

  $form['data']['file2'] = [
    '#type' => 'file',
    '#title' => t('Index File'),
    '#states' => [
      'visible' => [
        ':input[name="file_type"]' => ['value' => 'vcf'],
      ],
    ],
  ];

  $form['data']['file_path'] = [
    '#type' => 'textfield',
    '#title' => t('- OR Path to File on Server -'),
    '#description' => t('This path will be ignored if a file is provided above. Ex: sites/default/files/file.fasta or /data/file.fasta'),
    '#states' => [
      'invisible' => [
        ':input[name="file_type"]' => ['value' => 'vcf'],
      ],
    ],
  ];

  $form['data']['dir_path'] = [
    '#type' => 'textfield',
    '#title' => t('- OR Path to Directory on Server -'),
    '#description' => t('This path will be ignored if a file is provided above. ' . 'The directory must contain both the .tbi and .gz files.'),
    '#states' => [
      'visible' => [
        ':input[name="file_type"]' => ['value' => 'vcf'],
      ],
    ],
  ];

  $form['data']['symbolic_link'] = [
    '#type' => 'checkbox',
    '#title' => t('Symbolic Link'),
    '#description' => t('Create a symbolic link rather than make a copy of the file.'),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Add New Track',
  ];
  return $form;
}

/**
 * Validate the add track form.
 *
 * This function also takes care of the uploading of files.
 * TODO: If the file is invalid, delete it!
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The form state.
 */
function tripal_jbrowse_mgmt_add_track_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $file = $_FILES['files']['tmp_name']['file'];
  $settings = tripal_jbrowse_mgmt_get_settings();
  $instance = tripal_jbrowse_mgmt_get_instance($values['instance_id']);
  $data = $settings['data_dir'];
  $file_type = $values['file_type'];
  $symbolic_link = $values['symbolic_link'];
  $path = NULL;

  $base_path = $data . '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $base_path = $base_path . '_' . $instance->analysis_id;
  }
  $base_path = $base_path . '/data';

  if ($file_type === 'vcf') {
    $path = $base_path . '/vcf';
  }
  elseif ($file === 'bw') {
    $path = $base_path . '/wig';
  }
  else {
    $path = $base_path;
  }

  switch ($file_type) {
    case 'vcf':
      $index = $_FILES['files']['tmp_name']['file2'];
      $local_dir = isset($values['dir_path']) ? $values['dir_path'] : NULL;

      if (empty($file) && empty($index) && empty($local_dir)) {
        form_set_error('file',
          'Please provide a local directory path or upload files.');
      }
      elseif (empty($file) && empty($index) && !empty($local_dir)) {
        if (!file_exists($local_dir)) {
          form_set_error('file_path', 'The directory provided does not exist.');
        }
        else {
          if (!is_dir($local_dir)) {
            form_set_error('file_path',
              'The file provided is not a directory.');
          }
          else {
            $file_gz = glob($local_dir . '/*.vcf.gz');
            $file_index = glob($local_dir . '/*.vcf.gz.[cti][sbd][ix]');
            if (count($file_gz) != 1 || count($file_index) != 1) {
              form_set_error('file_path',
                'Please provide a directory with exactly one gz and one index file.');
            }
            else {
              try {
                if (!tripal_jbrowse_mgmt_copy_file($file_gz[0], $path, $symbolic_link)) {
                  form_set_error('file_path', 'Failed to copy file' . $file_gz[0] . ' to ' . $path);
                }
                else {
                  if (!tripal_jbrowse_mgmt_copy_file($file_index[0], $path, $symbolic_link)) {
                    form_set_error('file_path', 'Failed to copy file' . $file_gz[0] . ' to ' . $path);
                  }
                }
              } catch (Exception $exception) {
                form_set_error($exception->getMessage());
              }

            }
          }
        }
      }
      elseif (empty($file) && !empty($index)) {
        form_set_error('file', 'Please upload both a gz and an index file.');
      }
      elseif (!empty($file) && empty($index)) {
        form_set_error('file2', 'Please upload both a gz and an index file.');
      }
      else {
        $gz_uploaded = tripal_jbrowse_mgmt_upload_file('file');
        if (!$gz_uploaded) {
          form_set_error('file', 'Unable to upload file');
        }
        else {
          $gz_uploaded = tripal_jbrowse_mgmt_move_file($gz_uploaded, $path);
          if (!isset($gz_uploaded)) {
            form_set_error('file', 'Failed to move gz file to ' . $path . '.');
          }
          else {
            $form_state['values']['uploaded_gz'] = $gz_uploaded;
          }
        }

        $index_uploaded = tripal_jbrowse_mgmt_upload_file('file2');
        if (!$index_uploaded) {
          form_set_error('file2', 'Unable to upload file');
        }
        else {
          $index_uploaded = tripal_jbrowse_mgmt_move_file($index_uploaded, $path);
          if (!isset($index_uploaded)) {
            form_set_error('file2', 'Failed to move index file to ' . $path . '.');
          }
          else {
            $form_state['values']['uploaded_tbi'] = $index_uploaded;
          }
        }
      }

      break;

    default:
      $local_file = isset($values['file_path']) ? $values['file_path'] : NULL;

      if (empty($file) && empty($local_file)) {
        form_set_error('file',
          'Please provide a local file path or upload a new file.');
      }
      elseif (empty($file) && !empty($local_file)) {
        if (!file_exists($local_file)) {
          form_set_error('file_path', 'The file path provided does not exist.');
        }
        else {
          try {
            if (!tripal_jbrowse_mgmt_copy_file($local_file, $path, $symbolic_link)) {
              form_set_error('file_path', 'Failed to copy file ' . $local_file . ' to ' . $path);
            }
          } catch (Exception $exception) {
            form_set_error('file_path', 'Failed to copy file ' . $local_file . ' to ' . $path);
          }
        }
      }
      else {
        $uploaded = tripal_jbrowse_mgmt_upload_file('file');
        if (!$uploaded) {
          form_set_error('file', 'Unable to upload file');
        }
        else {
          $uploaded = tripal_jbrowse_mgmt_move_file($uploaded, $path);
          if (!isset($uploaded)) {
            form_set_error('file', 'Failed to move file to ' . $path);
          }
          else {
            $form_state['values']['uploaded_file'] = $uploaded;
          }
        }
      }

      break;
  }
}

/**
 * Handle form submission for adding a track.
 *
 * @param array $form
 * @param array $form_state
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_add_track_form_submit($form, &$form_state) {
  global $user;
  dpm($form_state, '$form_state in tripal_jbrowse_mgmt_add_track_form_submit');
  $values = $form_state['values'];

  $file = isset($values['file_path']) ? $values['file_path'] : NULL;
  if (!empty($values['dir_path'])) {
    $file = $values['dir_path'];
  }
  if (!empty($values['uploaded_gz'])) {
    $file = $values['uploaded_gz'];
  }
  if (!empty($values['uploaded_file'])) {
    $file = $values['uploaded_file'];
  }

  $instance = tripal_jbrowse_mgmt_get_instance($values['instance_id']);

  $track_id = tripal_jbrowse_mgmt_create_track($instance, [
    'label' => $values['label'],
    'track_type' => $values['track_type'],
    'file_type' => $values['file_type'],
    'file' => $file,
    'created_at' => time(),
  ]);

  tripal_add_job('Add JBrowse track to ' . $instance->title, 'tripal_jbrowse_mgmt',
    'tripal_jbrowse_mgmt_add_track_to_instance', [$track_id], $user->uid);

  drupal_goto('admin/tripal/extension/tripal_jbrowse/management/instances/' . $instance->id);
}

/**
 * Delete a track form.
 *
 * @param array $form
 * @param array $form_state
 * @param int $track_id
 *
 * @return array
 */
function tripal_jbrowse_mgmt_delete_track_form($form, &$form_state, $track_id) {

  $track = tripal_jbrowse_mgmt_get_track($track_id);

  if (!$track->id) {
    $form['error'] = [
      '#type' => 'item',
      '#markup' => '<p style="color: red">Unable to find track.</p>',
    ];

    return $form;
  }

  $form['description'] = [
    '#type' => 'item',
    '#markup' => 'Are you sure you want to delete the ' . $track->label . ' track?',
  ];

  $form['track_id'] = [
    '#type' => 'hidden',
    '#value' => $track_id,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Delete Track',
  ];

  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l('Cancel',
      'admin/tripal/extension/tripal_jbrowse/management/instances/' . $track->instance_id),
  ];

  return $form;
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_delete_track_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];

  $track = tripal_jbrowse_mgmt_get_track($values['track_id']);

  tripal_add_job('Delete JBrowse track', 'tripal_jbrowse_mgmt',
    'tripal_jbrowse_mgmt_delete_track_from_instance', [$values['track_id']],
    $user->uid);

  tripal_jbrowse_mgmt_update_track($track, ['is_deleted' => 1]);

  drupal_goto('admin/tripal/extension/tripal_jbrowse/management/instances/' . $track->instance_id);
}

/**
 * Track json editor advance form.
 * allow user to make all configurations of a track
 *
 * @param array $form
 * @param array $form_state
 * @param int $track_id
 *
 * @return array
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_json_editor_advance_form($form, &$form_state, $track_id) {
  $track = tripal_jbrowse_mgmt_get_track($track_id);

  if (!$track->id) {
    $form['error'] = [
      '#type' => 'item',
      '#markup' => '<p style="color: red">Unable to find track.</p>',
    ];

    return $form;
  }

  $instance = tripal_jbrowse_mgmt_get_instance($track->instance_id);
  $json = tripal_jbrowse_mgmt_get_json($instance);
  $form_state['track_json'] = $json;

  $key = tripal_jbrowse_mgmt_make_slug($track->label);
  $track_json = NULL;
  $track_index = NULL;
  foreach ($json['tracks'] as $index => $jtrack) {
    if ($jtrack['label'] === $key) {
      $track_json = $jtrack;
      $track_index = $index;
      break;
    }
  }

  if (!$track_json) {
    $form['error'] = [
      '#type' => 'item',
      '#markup' => '<p style="color: red">Unable to find track in json!</p>',
    ];
    return $form;
  }

  $form['track_index'] = [
    '#type' => 'hidden',
    '#value' => $track_index,
  ];

  $form['track_id'] = [
    '#type' => 'hidden',
    '#value' => $track->id,
  ];

  $instr_detail = '<p><strong>Only use this functionality if the configuration option you want is not included in the "Track Manage" form.</strong>. For more information on the JBrowse track configuration see the '.l('JBrowse Documentation', 'http://jbrowse.org/docs/canvas_features.html').'.</p>
    <div class="messages warning">Please be extra cautious while editing track configuration, since it will make changes in file trackList.json directly. Also, do not change the "label" since it will disconnect this track from the JBrowse Management form.</div>';

  $form['Instruction']=[
    '#type' => 'markup',
    '#markup' => $instr_detail,
  ];

  $form['track_all_config'] = [
    '#type' => 'textarea',
    '#title' => t('Track configuration in json'),
    '#description' => t('The track info from trackList.json. Only '),
    '#default_value' => json_encode($track_json, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES),
    '#rows' => '10',
    '#required' => TRUE,
  ];


  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save Track Configuration',
  ];

  return $form;
}

/**
 * Validate the form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_jbrowse_mgmt_json_editor_advance_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $track_all_config = $values['track_all_config'] ?? NULL;
  $track_key = $values['track_index'];

  if ($track_all_config && !empty($track_all_config)) {
    if (!json_decode($track_all_config)) {
      form_set_error(
        'track_all_config',
        'Invalid JSON. Please verify that the menu template contains only valid JSON.'
      );
    }
  }
  $json_before_edit = $form_state['track_json']['tracks'][$track_key];
  $json_after_edit = json_decode($form_state['values']['track_all_config'], TRUE);
  if (($json_before_edit['key'] !=  $json_after_edit['key']) OR ($json_before_edit['label'] != $json_after_edit['label'])){
    form_set_error('track_all_config', 'Key or Label changed. Please don\'t change Key or Label here. The functionality is provided in "Track Manage" form.');
  }


}

/**
 * @param array $form
 * @param array $form_state
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_json_editor_advance_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $track_index = $values['track_index'];
  $track_all_config = $values['track_all_config'] ?? NULL;

  $track = tripal_jbrowse_mgmt_get_track($values['track_id']);

  $json = $form_state['track_json'];
  $json['tracks'][$track_index] = json_decode($track_all_config, TRUE);
  tripal_jbrowse_mgmt_update_track($track, ['label' => $json['tracks'][$track_index]['key']]);

  if (tripal_jbrowse_mgmt_save_json($track->instance, $json) === FALSE) {
    drupal_set_message(
      'Unable to save JSON file. Please make sure it\'s editable.'
    );
    return;
  }

  drupal_set_message('Track updated successfully');
}
