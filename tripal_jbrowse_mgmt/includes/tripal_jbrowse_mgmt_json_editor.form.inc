<?php

/**
 * Track json editor form.
 *
 * @param array $form
 * @param array $form_state
 * @param int $track_id
 *
 * @return array
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_json_editor_form($form, &$form_state, $track_id) {
  $track = tripal_jbrowse_mgmt_get_track($track_id);
  if (empty($track)) {
    drupal_not_found();
    return $form;
  }

  drupal_set_title('Edit Track: ' . $track->label);

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

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t('Change the label'),
    '#default_value' => $track->label,
    '#required' => TRUE,
  ];

  $form['category'] = [
    '#type' => 'textfield',
    '#title' => t('Category'),
    '#description' => t('Customize the category'),
    '#default_value' => $track_json['metadata']['category'] ?? '',
  ];

  $track_type = $track_json['trackType'] ?? $track_json['type'];

  $form['track_type'] = [
    '#type' => 'select',
    '#title' => t('Track Type'),
    '#description' => t(
      'See the jborwse documentation for information about types'
    ),
    '#options' => drupal_map_assoc(
      hardwoods_get_track_types()
    ),
    '#default_value' => $track_type,
  ];

  $form['menu_template'] = [
    '#type' => 'textarea',
    '#title' => t('Menu Template'),
    '#description' => t('Right click menu template'),
    '#default_value' => json_encode(
      $track_json['menuTemplate'] ?? new stdClass(),
      JSON_PRETTY_PRINT
    ),
  ];

  $form['style'] = [
    '#type' => 'fieldset',
    '#title' => t('Track Style'),
  ];

  $form['style']['classname'] = [
    '#type' => 'textfield',
    '#title' => t('CSS Class Name'),
    '#description' => t(
      'Add custom css class names. Separate multiple classes using spaces.'
    ),
    '#default_value' => $track_json['style']['className'] ?? '',
  ];

  $form['style']['color'] = [
    '#type' => 'textfield',
    '#title' => t('Color'),
    '#description' => t(
      'Track background color. Examples: #00ff00 or rgba(0, 255, 0, 1).'
    ),
    '#default_value' => $track_json['style']['color'] ?? '',
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
function tripal_jbrowse_mgmt_json_editor_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $menu_template = $values['menu_template'] ?? NULL;

  if ($menu_template && !empty($menu_template)) {
    if (!json_decode($menu_template)) {
      form_set_error(
        'menu_template',
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
function tripal_jbrowse_mgmt_json_editor_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $category = $values['category'] ?? NULL;
  $menu_template = $values['menu_template'] ?? NULL;
  $color = $values['color'] ?? NULL;
  $classname = $values['classname'] ?? NULL;
  $track_index = $values['track_index'];
  $track_type = $values['track_type'];

  $track = tripal_jbrowse_mgmt_get_track($values['track_id']);
  tripal_jbrowse_mgmt_update_track($track, ['label' => $values['label']]);

  $json = $form_state['track_json'];
  $track_json = $json['tracks'][$track_index];

  $track_json['label'] = tripal_jbrowse_mgmt_make_slug($values['label']);
  $track_json['key'] = $values['label'];
  if (isset($track_json['trackType'])) {
    $track_json['trackType'] = $track_type;
  }

  if (isset($track_json['type'])) {
    $track_json['type'] = $track_type;
  }

  if ($category) {
    $track_json['metadata']['category'] = $category;
  }

  if ($menu_template) {
    $menu_template = json_decode($menu_template, TRUE);
    $track_json['menuTemplate'] = $menu_template;
    if (empty($track_json['menuTemplate'])) {
      unset($track_json['menuTemplate']);
    }
  }

  if ($color) {
    $track_json['style']['color'] = $color;
  }

  if ($classname) {
    $track_json['style']['className'] = $classname;
  }

  $json['tracks'][$track_index] = $track_json;

  if (tripal_jbrowse_mgmt_save_json($track->instance, $json) === FALSE) {
    drupal_set_message(
      'Unable to save JSON file. Please make sure it\'s editable.'
    );
    return;
  }

  drupal_set_message('Track updated successfully');
}
