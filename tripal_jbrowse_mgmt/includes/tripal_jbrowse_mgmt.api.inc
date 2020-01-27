<?php

/**
 * @file
 */

/**
 * Get saved settings.
 *
 * @return null
 */
function tripal_jbrowse_mgmt_get_settings() {
  $default = [
    'bin_path' => '',
    'link' => '',
    'data_dir' => '',
    'data_path' => '',
    'menu_template' => [],
  ];

  $variable = variable_get('tripal_jbrowse_mgmt_settings', json_encode($default));
  $settings = json_decode($variable, TRUE) + $default;

  return $settings;
}

/**
 * Save settings.
 *
 * @param array $settings
 *
 * @return array The final merged settings
 */
function tripal_jbrowse_mgmt_save_settings($settings) {
  $default = [
    'bin_path' => '',
    'link' => '',
    'data_dir' => '',
    'menu_template' => [],
  ];

  $final = $settings + $default;

  variable_set('tripal_jbrowse_mgmt_settings', json_encode($final));

  return $final;
}

/**
 * Get an array to instances.
 *
 * @return mixed
 */
function tripal_jbrowse_mgmt_get_instances($conditions = NULL) {
  static $users = [];
  static $organisms = [];
  static $analysis = [];

  $instances = db_select('tripal_jbrowse_mgmt_instances', 'H')->fields('H');

  if ($conditions) {
    foreach ($conditions as $field => $value) {
      $instances->condition($field, $value);
    }
  }

  $instances = $instances->execute()->fetchAll();

  foreach ($instances as $key => &$instance) {
    if (!isset($users[$instance->uid])) {
      $users[$instance->uid] = user_load($instance->uid);
    }
    $instance->user = $users[$instance->uid];

    if (!isset($organisms[$instance->organism_id])) {
      $organisms[$instance->organism_id] = chado_query('SELECT * FROM {organism} WHERE organism_id=:id',
        [':id' => $instance->organism_id])->fetchObject();
    }
    $instance->organism = $organisms[$instance->organism_id];

    if($instance->analysis_id){
      if (!isset($analysis[$instance->analysis_id])){
        $analysis[$instance->analysis_id] = chado_query('SELECT * FROM {analysis} WHERE analysis_id=:id',
          [':id' => $instance->analysis_id])->fetchObject();
      }
      $instance->analysis = $analysis[$instance->analysis_id];
    }

  }
  return $instances;
}

/**
 * Create a new instance.
 *
 * @param $data
 *
 * @return \DatabaseStatementInterface|int
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_create_instance($data) {
  global $user;
  // Check that required fields are provided.
  $required = ['organism_id', 'created_at'];
  foreach ($required as $key) {
    if (!isset($data[$key])) {
      tripal_report_error(
        'tripal_jbrowse_mgmt',
        TRIPAL_ERROR,
        'Unable to create instance due to missing :key',
        [':key' => $key]
      );
      return FALSE;
    }
  }

  // If they are all present, then insert into the database.
  return db_insert('tripal_jbrowse_mgmt_instances')->fields([
    'uid' => $user->uid,
    'organism_id' => $data['organism_id'],
    'analysis_id' => $data['analysis_id'],
    'title' => $data['title'],
    'description' => isset($data['description']) ? $data['description'] : '',
    'created_at' => $data['created_at'],
    'file' => $data['file'],
  ])->execute();

}

/**
 * Get an instance by id.
 *
 * @param $instance_id
 *
 * @return mixed
 */
function tripal_jbrowse_mgmt_get_instance($instance_id) {
  $instance = tripal_jbrowse_mgmt_get_instances(['id' => $instance_id]);

  return reset($instance);
}

/**
 * Get analysis_id from string(autocomplete analysis)
 *
 * @param $analysis_string (in format: ******** (###))
 *
 * @return int
 */
function tripal_jbrowse_mgmt_get_analysis_id_from_string($analysis_string){
  preg_match_all('!\d+!', $analysis_string, $match_analysis);
  $analysis_id =  array_pop($match_analysis[0]);
  return $analysis_id;
}


/**
 * Update an instance.
 *
 * @param int $id
 * @param array $data
 *
 * @return \DatabaseStatementInterface
 */
function tripal_jbrowse_mgmt_update_instance($id, $data) {
  return db_update('tripal_jbrowse_mgmt_instances')
    ->fields($data)
    ->condition('id', $id)
    ->execute();
}

/**
 * @param int|object $instance
 *
 * @return int
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_delete_instance($instance) {
  if (is_object($instance) && property_exists($instance, 'id')) {
    $id = $instance->id;
  }
  elseif (is_numeric($instance)) {
    $id = $instance;
  }
  else {
    throw new Exception('Unable to extract instance ID. Please provide a valid ID to delete the instance.');
  }

  return db_delete('tripal_jbrowse_mgmt_instances')
    ->condition('id', $id)
    ->execute();
}

/**
 * Create a new JBrowse track for a given instance.
 *
 * @param $data
 *
 * @return bool|int Track ID or FALSE if an error occurs.
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_create_track($instance, $data) {
  global $user;

  return db_insert('tripal_jbrowse_mgmt_tracks')->fields([
    'uid' => $user->uid,
    'instance_id' => $instance->id,
    'organism_id' => $instance->organism_id,
    'label' => $data['label'],
    'track_type' => $data['track_type'],
    'file_type' => $data['file_type'],
    'created_at' => $data['created_at'],
    'file' => $data['file'],
  ])->execute();
}

/**
 * Delete a track by ID.
 *
 * @param $track_id
 *
 * @return int
 */
function tripal_jbrowse_mgmt_delete_track($track_id) {
  return db_delete('tripal_jbrowse_mgmt_tracks')
    ->condition('id', $track_id)
    ->execute();
}

/**
 * Get attached tracks with users pre-loaded.
 *
 * @param $instance
 *
 * @return mixed
 */
function tripal_jbrowse_mgmt_get_tracks($instance, array $conditions = []) {
  static $users = [];

  $tracks = db_select('tripal_jbrowse_mgmt_tracks', 'HJT')->fields('HJT');

  foreach ($conditions as $field => $value) {
    $tracks->condition($field, $value);
  }

  $tracks = $tracks->condition('instance_id', $instance->id)
    ->execute()
    ->fetchAll();

  foreach ($tracks as &$track) {
    if (!isset($users[$track->uid])) {
      $users[$track->uid] = user_load($track->uid);
    }

    $track->user = $users[$track->uid];
  }

  return $tracks;
}

/**
 * Get a track with instance and user data attached.
 *
 * @param $track_id
 *
 * @return mixed
 */
function tripal_jbrowse_mgmt_get_track($track_id) {
  $track = db_select('tripal_jbrowse_mgmt_tracks', 'HJT')
    ->fields('HJT')
    ->condition('id', $track_id)
    ->execute()
    ->fetchObject();

  $track->user = user_load($track->uid);
  $track->instance = tripal_jbrowse_mgmt_get_instance($track->instance_id);

  return $track;
}

/**
 * @param $track
 * @param array $fields
 *
 * @return \DatabaseStatementInterface
 */
function tripal_jbrowse_mgmt_update_track($track, array $fields) {
  return db_update('tripal_jbrowse_mgmt_tracks')
    ->fields($fields)
    ->condition('id', $track->id)
    ->execute();
}

/**
 * Get a list of organisms.
 *
 * @return mixed
 */
function tripal_jbrowse_mgmt_get_organisms_list() {
  return db_select('chado.organism', 'CO')
    ->fields('CO', ['organism_id', 'genus', 'species', 'common_name'])
    ->execute()
    ->fetchAll();
}

/**
 * parse analysis to obtain analysis_id
 *
 * @return mixed
 */
function tripal_jbrowse_mgmt_analysis_parse_4id($name_id) {


  return $analysis_id;
}


/**
 * Format the name of the organism to `Genus species (Common Name)`.
 *
 * @param object $organism
 *   The organism object as retrieved by
 *   db_query()->fetchObject().
 *
 * @return string
 */
function tripal_jbrowse_mgmt_construct_organism_name($organism) {
  $name = $organism->genus;
  $name .= " $organism->species";

  if (!empty($organism->common_name)) {
    $name .= " ($organism->common_name)";
  }

  return $name;
}

/**
 * Sanitize the string for the URL.
 *
 * @param string $string
 *   The string to sanitize.
 *
 * @return string
 *   The sanitized string.
 */
function tripal_jbrowse_mgmt_make_slug($string) {
  $slug = str_replace(' ', '_', $string);
  $slug = str_replace('(', '_', $slug);
  $slug = str_replace(')', '_', $slug);
  $slug = str_replace('[', '_', $slug);
  $slug = str_replace(']', '_', $slug);
  $slug = str_replace('!', '_', $slug);
  $slug = str_replace('?', '_', $slug);
  $slug = str_replace('"', '_', $slug);
  $slug = str_replace('\'', '_', $slug);
  $slug = str_replace('\\', '_', $slug);
  $slug = str_replace(':', '_', $slug);

  return strtolower(trim($slug, '_'));
}

/**
 * @param $field
 *
 * @return bool|\stdClass
 */
function tripal_jbrowse_mgmt_upload_file($field) {
  $file = file_save_upload($field, [
    'file_validate_extensions' => ['fasta faa fna fastq txt gff vcf wig gz tbi bw'],
    // Make it 20 GB max.
    'file_validate_size' => [1024 * 1024 * 1024 * 20],
  ]);

  // drupal_realpath($file->uri);.
  return !$file ? FALSE : $file;
}

/**
 * Moves a file to an intermediate directory, then to the destination, if given.
 *
 * @param $file
 *   The file object.
 *
 * @param $path
 *   The path to the directory of the new object.
 *   If the directory provided does not exist, it will be created.
 *
 * @return
 *   The path to the moved file, or NULL on fail.
 */
function tripal_jbrowse_mgmt_move_file($file, $path = NULL) {
  $directory = 'public://tripal/tripal_jbrowse_mgmt';
  file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
  $file = file_move($file, $directory, FILE_EXISTS_REPLACE);

  if (isset($path)) {
    file_prepare_directory($path, FILE_CREATE_DIRECTORY);
    $oldname = drupal_realpath($file->uri);
    $newname = $path . '/' . $file->filename;
    if (!rename($oldname, $newname)) {
      return NULL;
    }
  }

  return isset($path) ? $newname : drupal_realpath($file->uri);
}

/**
 * Copy a file into a new directory.
 * If the directory provided does not exist, it will be created.
 *
 * @param $source
 *   File path of the source file.
 * @param $destination
 *   File path to the destination directory.
 * @param $symbolic_link
 *   bool indicates create symbolic_link or copy
 * @return bool
 */
function tripal_jbrowse_mgmt_copy_file($source, $destination, $sym_link) {
  if (empty($destination)) {
    throw new Exception('Please provide a valid destination path to copy the source to.');
  }

  file_prepare_directory($destination, FILE_CREATE_DIRECTORY);
  if (isset($sym_link) AND ($sym_link == true)){
    $destination_symlink = $destination.'/'.basename($source);
    return symlink($source, $destination_symlink);
  }
  else{
    return file_unmanaged_copy($source, $destination, FILE_EXISTS_ERROR);
  }

}


/**
 * Build the http query for a given instance to link to JBrowse.
 *
 * @param $instance
 *
 * @return array
 */
function tripal_jbrowse_mgmt_build_http_query($instance) {
  $path = tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $path = $path . '_' . $instance->analysis_id;
  }
  $added_tracks = tripal_jbrowse_mgmt_get_tracks($instance);
  $properties = tripal_jbrowse_mgmt_get_instance_properties($instance->id);

  // Determine the tracks to display.
  $tracks_path = '';
  // If the start-tracks property is set then use that directly.
  if (isset($properties['start-tracks'])) {
    $tracks_path = $properties['start-tracks'];
  }
  // Otherwise, show all tracks that were added using this module.
  elseif (!empty($added_tracks)) {
    $tracks_path = implode(',', array_map(function ($track) {
      return tripal_jbrowse_mgmt_make_slug($track->label);
    }, $added_tracks));
  }

  $settings = tripal_jbrowse_mgmt_get_settings();
  $data_path = (empty($settings['data_path'])) ? 'data' : '/' . $settings['data_path'];

  return [
    'data' => "$data_path/$path/data",
    'tracks' => $tracks_path,
  ];

  // Now we need to add start location if it's set.
  if (isset($properties['start-loc'])) {
    $query_params['loc'] = $properties['start-loc'];
  }

  return $query_params;
}

/**
 * Get trackList.json for an instance.
 *
 * @param object $instance
 *
 * @return array Decoded json array.
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_get_json($instance) {
  $path = tripal_jbrowse_mgmt_get_track_list_file_path($instance);

  $contents = file_get_contents($path);
  if (!$contents) {
    throw new Exception('Unable to find ' . $path . ' file');
  }

  return json_decode($contents, TRUE);
}

/**
 * Get the json for a given track.
 *
 * @param object $track
 *    The track from the database.
 *
 * @return array
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_get_track_json($track) {
  if (!$track->instance) {
    $track->instance = tripal_jbrowse_mgmt_get_instance($track->instance_id);
  }

  $json = tripal_jbrowse_mgmt_get_json($track->instance);
  $key = tripal_jbrowse_mgmt_make_slug($track->label);
  $track_json = NULL;
  foreach ($json['tracks'] as $index => $jtrack) {
    if ($jtrack['label'] === $key) {
      $track_json = $jtrack;
      break;
    }
  }

  return $track_json;
}

/**
 * @param object $track
 *   Track object.
 * @param array $track_json
 *   Edited track array.
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_save_track_json($track, $track_json) {
  if (!$track->instance) {
    $track->instance = tripal_jbrowse_mgmt_get_instance($track->instance_id);
  }

  $json = tripal_jbrowse_mgmt_get_json($track->instance);
  $key = tripal_jbrowse_mgmt_make_slug($track->label);
  foreach ($json['tracks'] as $index => $jtrack) {
    if ($jtrack['label'] === $key) {
      $json['tracks'][$index] = $track_json;
      break;
    }
  }

  return tripal_jbrowse_mgmt_save_json($track->instance, $json);
}

/**
 * @param object $instance
 * @param array $data
 *
 * @throws \Exception
 * @return bool|int
 */
function tripal_jbrowse_mgmt_save_json($instance, $data) {
  $path = tripal_jbrowse_mgmt_get_track_list_file_path($instance);

  $default = tripal_jbrowse_mgmt_get_json($instance);
  $json = $data + $default;
  $encoded = json_encode($json, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  return file_put_contents($path, $encoded);
}

/**
 * @param $instance
 *
 * @return string
 */
function tripal_jbrowse_mgmt_get_track_list_file_path($instance) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  $path = $settings['data_dir'];
  $path .= '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $path = $path . '_' . $instance->analysis_id;
  }
  $path .= '/data/trackList.json';

  return $path;
}

/**
 * Gets a list of supported track types.
 *
 * @return array
 */
function tripal_jbrowse_mgmt_get_track_types() {
  return [
    'FeatureTrack',
    'CanvasFeatures',
    'HTMLFeatures',
    'HTMLVariants',
    'XYPlot',
  ];
}

/**
 * Save properties for a given instance.
 *
 * @param $id
 *   The instance ID that properties should be associated with.
 * @param $data
 *   An array of properties where each is
 *   key: property_type, value: property value.
 */
function tripal_jbrowse_mgmt_save_instance_properties($id, $data) {

  // For each property...
  foreach ($data as $type => $value) {
    tripal_jbrowse_mgmt_save_instance_property($id, $type, $value);
  }

}

/**
 * Save the details for a specific property of a given instance.
 *
 * @param $id
 *   The instance ID that properties should be associated with.
 * @param $type
 *   The name of the property type to be saved.
 * @param $value
 *   The value of the property to be saved.
 */
function tripal_jbrowse_mgmt_save_instance_property($id, $type, $value) {

  // Check to see if the property already exists.
  $result = db_select('tripal_jbrowse_mgmt_instanceprop', 'p')
    ->fields('p', ['value'])
    ->condition('instance_id', $id)
    ->condition('property_type', $type)
    ->execute()->fetchObject();

  // If the property doesn't already exist then insert it.
  if (empty($result)) {
    return db_insert('tripal_jbrowse_mgmt_instanceprop')
      ->fields([
        'instance_id' => $id,
        'property_type' => $type,
        'value' => $value,
      ])->execute();
  }
  // Otherwise, we need to update it.
  else {
    return db_update('tripal_jbrowse_mgmt_instanceprop')
      ->fields([
        'value' => $value,
      ])
      ->condition('instance_id', $id)
      ->condition('property_type', $type)
      ->execute();
  }
}

/**
 * Retrieves the value for a given instance property.
 *
 * @param $id
 *   The instance ID.
 * @param $type
 *   The name of the property type.
 * @return
 *   The value of the property for a given instance.
 */
function tripal_jbrowse_mgmt_get_instance_property($id, $type) {
  return db_select('tripal_jbrowse_mgmt_instanceprop', 'p')
    ->fields('p', ['value'])
    ->condition('instance_id', $id)
    ->condition('property_type', $type)
    ->execute()->fetchField();
}

/**
 * Retrieves all properties for a given instance.
 *
 * @param $id
 *   The instance ID.
 * @return
 *   An array of properties for the specified instance
 *   where the key: property type and value: property value.
 */
function tripal_jbrowse_mgmt_get_instance_properties($id) {
  return db_select('tripal_jbrowse_mgmt_instanceprop', 'p')
    ->fields('p', ['property_type', 'value'])
    ->condition('instance_id', $id)
    ->execute()->fetchAllKeyed(0,1);
}
