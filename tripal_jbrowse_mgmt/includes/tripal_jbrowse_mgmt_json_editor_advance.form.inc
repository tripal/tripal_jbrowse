<?php

/**
 * Track json editor advance form.
 * allow user to make advanced changes
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
  if (empty($track)) {
    drupal_not_found();
    return $form;
  }

  drupal_set_title('Edit Track Advance: ' . $track->label);

  $breadcrumb = array();
  $breadcrumb[] = l('Home', '');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal JBrowse', 'admin/tripal/extension/tripal_jbrowse/management');
  $breadcrumb[] = l('Instance', 'admin/tripal/extension/tripal_jbrowse/management/tracks/'.$track->instance_id);
  $breadcrumb[] = l('Instance', 'admin/tripal/extension/tripal_jbrowse/management/tracks/'.$track->instance_id.'/edit_json');
  drupal_set_breadcrumb($breadcrumb);

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
  //@TEST
  dpm($track_json, '$track_json');
  dpm($track_index, '$track_index');
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
  //@TEST
  dpm($form_state, '$form_state in validation');
  $values = $form_state['values'];
  $track_all_config = $values['track_all_config'] ?? NULL;

  if ($track_all_config && !empty($track_all_config)) {
    if (!json_decode($track_all_config)) {
      form_set_error(
        'track_all_config',
        'Invalid JSON. Please verify that the menu template contains only valid JSON.'
      );
    }
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
  //@TEST
  dpm($json, '$json in submit');
  if (tripal_jbrowse_mgmt_save_json($track->instance, $json) === FALSE) {
    drupal_set_message(
      'Unable to save JSON file. Please make sure it\'s editable.'
    );
    return;
  }

  drupal_set_message('Track updated successfully');
}
