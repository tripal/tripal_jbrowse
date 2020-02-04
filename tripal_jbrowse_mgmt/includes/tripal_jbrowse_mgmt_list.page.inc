<?php

/**
 * @return array
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_instances_page() {
  $instances = tripal_jbrowse_mgmt_get_instances();
  $settings = tripal_jbrowse_mgmt_get_settings();

  // Ensure people set settings before adding an instance!
  if (empty($settings['data_dir'])) {
    drupal_set_message(t(
      'The <a href="@url">Location of your JBrowse</a> is required before creating an instance.',
        ['@url' => url('admin/tripal/extension/tripal_jbrowse/management/configure')]),
      'error');
  }

  $content = [];

  if (empty($instances)) {
    $content['empty'] = [
      '#type' => 'item',
      '#markup' => 'There are no instances at this time. Please use the link above to create a new one.',
    ];

    return $content;
  }

  $header = [
    'Organism',
    'Analysis',
    'Submitter',
    'Description',
    'Tracks',
    'Launch',
    'Instance',
  ];

  $rows = [];
  foreach ($instances as $instance) {
    $rows[] = [
      l(
        $instance->title,
        'admin/tripal/extension/tripal_jbrowse/management/instances/' . $instance->id
      ),
      $instance->analysis->name ?: 'Not provided',
      $instance->user->name,
      $instance->description ?: 'Not provided',
      l('Manage Tracks', 'admin/tripal/extension/tripal_jbrowse/management/instances/' . $instance->id),
      l(
        'Launch',
        $settings['link'],
        [
          'query' => tripal_jbrowse_mgmt_build_http_query($instance),
          'attributes' => ['target' => '_blank'],
        ]
      ),
      l('Edit', 'admin/tripal/extension/tripal_jbrowse/management/instances/'.$instance->id.'/edit') . ' <br /> ' .
      l('Delete', 'admin/tripal/extension/tripal_jbrowse/management/instances/'.$instance->id.'/delete'),

    ];
  }

  $content['list'] = [
    '#type' => 'markup',
    '#markup' => theme(
      'table',
      [
        'rows' => $rows,
        'header' => $header,
      ]
    ),
  ];

  return $content;
}
