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

  $breadcrumb = array();
  $breadcrumb[] = l('Home', '');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal JBrowse', 'admin/tripal/extension/tripal_jbrowse/management');
  drupal_set_breadcrumb($breadcrumb);

  $content = [];

  $content['instance_table'] = [
    '#type' => 'markup',
    '#markup' => theme(
      'table',
      [
        'header' => ['Key', 'Value'],
        'rows' => [
          ['Instance Name', $instance->title],
          ['Analysis Name', $instance->analysis->name ?: 'Not provided'],
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

  // First get tracks added by this module.
  $tracks = tripal_jbrowse_mgmt_get_tracks($instance, ['is_deleted' => 0]);

  $content['tracks_title'] = [
    '#type' => 'item',
    '#markup' => '<h4>Tracks</h4>',
  ];

  $managed_tracks = [];
  $rows = [];

  if (!empty($tracks)) {
    foreach ($tracks as $track) {

      // Keep track of managed tracks so they are not duplicated later.
      $managed_tracks[] = $track->label;

      // Add the current tracks to the table.
      $rows[] = [
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
    }
  }

  // Second grab all tracks from the JSON.
  $trackList = tripal_jbrowse_mgmt_get_json($instance);
  if (!empty($trackList) and isset($trackList['tracks'])) {

    foreach ($trackList['tracks'] as $track) {

      // Make sure we only get the readable component for the type.
      $path_type = explode('/', $track['type']);
      $type = end($path_type);

      // Make sure this is not a managed track.
      if (in_array($track['key'], $managed_tracks)) {
        continue;
      }

      // Now compile our row.
      $rows[] = [
        $track['key'],
        $type,
        '',
        '',
        '',
        '',
        '',
      ];
    }
  }

  // Finally, if there are tracks, show them in a table.
  if (!empty($rows)) {
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
  // Otherwise, prompt people to add tracks!
  else {
    $content['no_tracks'] = [
      '#type' => 'item',
      '#markup' => 'No tracks found for this instance. Please use the add tracks link above to add new tracks.',
    ];
  }

  return $content;
}
