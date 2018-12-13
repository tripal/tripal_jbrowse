<?php

/**
 * @param $instance_id
 *
 * @return mixed
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_instance_page($instance_id) {
  $instance = tripal_jbrowse_mgmt_get_instance($instance_id);

  if (empty($instance)) {
    drupal_not_found();
    return '';
  }

  $settings = tripal_jbrowse_mgmt_get_settings();

  drupal_set_title("Manage $instance->title JBrowse");

  $content = [];

  $content['instance_table'] = [
    '#type' => 'markup',
    '#markup' => theme(
      'table',
      [
        'header' => ['Key', 'Value'],
        'rows' => [
          ['Instance Name', $instance->title],
          ['Created At', date('m/d/Y', $instance->created_at)],
          [
            'Organism',
            "{$instance->organism->genus} {$instance->organism->species}",
          ],
          ['Created By', $instance->user->name],
          [
            'Launch',
            l(
              'See ' . $instance->title . ' on JBrowse',
              $settings['link'],
              ['query' => tripal_jbrowse_mgmt_build_http_query($instance)]
            ),
          ],
        ],
      ]
    ),
  ];

  $tracks = tripal_jbrowse_mgmt_get_tracks($instance, ['is_deleted' => 0]);

  $content['tracks_title'] = [
    '#type' => 'item',
    '#markup' => '<h4>Tracks</h4>',
  ];

  if (empty($tracks)) {
    $content['no_tracks'] = [
      '#type' => 'item',
      '#markup' => 'No tracks found for this instance. Please use the add tracks link above to add new tracks.',
    ];
  }
  else {
    $rows = array_map(
      function ($track) {
        return [
          $track->label,
          $track->track_type,
          $track->file_type,
          $track->user->name,
          date('m/d/Y', $track->created_at),
          l('Manage Track', 'admin/tripal/extension/tripal_jbrowse/management/tracks/' . $track->id),
          l(
            'Delete Track',
            'admin/tripal/extension/tripal_jbrowse/management/tracks/' . $track->id . '/delete'
          ),
        ];
      },
      $tracks
    );

    $content['tracks_table'] = [
      '#type' => 'markup',
      '#markup' => theme(
        'table',
        [
          'header' => [
            'Label',
            'Track Type',
            'File Type',
            'Created By',
            'Created At',
            'Manage',
            'Delete Track',
          ],
          'rows' => $rows,
        ]
      ),
    ];
  }

  return $content;
}
