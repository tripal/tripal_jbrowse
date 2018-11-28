<?php

/**
 * Create first set of files for the instance.
 * This simply means preparing the reference sequence.
 *
 * @param $instance_id
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_create_instance_files($instance_id) {
  $instance = tripal_jbrowse_mgmt_get_instance($instance_id);

  if (empty($instance)) {
    throw new Exception('Unable to find instance to create files for.');
  }

  $exit = tripal_jbrowse_mgmt_cmd_prepare_refseq($instance);
  if ($exit == 0) {
    if (tripal_jbrowse_mgmt_cmd_generate_names($instance) != 0) {
      throw new Exception(
        'Unable to generate names for the instance. See above for errors.'
      );
    }

    return;
  }

  throw new Exception(
    'Unable to prepare reference sequence for the instance. See above for
     errors.'
  );
}

/**
 * Job to create a new instance.
 *
 * @param $track_id
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_add_track_to_instance($track_id) {
  $track = tripal_jbrowse_mgmt_get_track($track_id);

  if (empty($track)) {
    throw new Exception('Unable to find instance to create files for.');
  }

  if (tripal_jbrowse_mgmt_cmd_add_track($track) != 0) {
    throw new Exception('Unable to add track. See errors above.');
  }
}

/**
 * Job to delete a track from an instance.
 *
 * @param $track_id
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_delete_track_from_instance($track_id)
{
  $track = tripal_jbrowse_mgmt_get_track($track_id);

  if (empty($track)) {
    throw new Exception('Unable to find instance to create files for.');
  }

  if (tripal_jbrowse_mgmt_cmd_delete_track($track) != 0) {
    tripal_jbrowse_mgmt_update_track($track, ['is_deleted' => 0]);
    throw new Exception('Unable to add track. See errors above.');
  }

  tripal_jbrowse_mgmt_delete_track($track_id);
}
