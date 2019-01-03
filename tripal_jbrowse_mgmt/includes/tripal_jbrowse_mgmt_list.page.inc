<?php

/**
 * @return array
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_instances_page() {
  $instances = tripal_jbrowse_mgmt_get_instances();
  $settings = tripal_jbrowse_mgmt_get_settings();

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
    'Submitter',
    'Description',
    'Manage',
    'Launch',
    'Delete',
  ];

  $rows = [];
  foreach ($instances as $instance) {
    $rows[] = [
      l(
        $instance->title,
        'admin/tripal/extension/tripal_jbrowse/management/instances/' . $instance->id
      ),
      $instance->user->name,
      $instance->description ?: 'Not provided',
      l('Manage Tracks', 'admin/tripal/extension/tripal_jbrowse/management/instances/' . $instance->id),
      l(
        'Launch',
        $settings['link'],
        ['query' => tripal_jbrowse_mgmt_build_http_query($instance)]
      ),
      l('Delete Instance', 'admin/tripal/extension/tripal_jbrowse/management/instances/'.$instance->id.'/delete'),

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
