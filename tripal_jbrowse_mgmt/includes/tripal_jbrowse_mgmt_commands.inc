<?php

/**
 * Add a reference sequence to the instance.
 *
 * @param $instance
 *
 * @return string
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_cmd_prepare_refseq($instance) {
  $settings = tripal_jbrowse_mgmt_get_settings();
  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];

  $path = $data . '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $path = $path . '_' . $instance->analysis_id;
  }

  if (!file_exists($path)) {
    if (!mkdir($path)) {
      throw new Exception(
        'Unable to make data directory! Please make sure the directory
      at ' . $data . ' exists and is writable by the current user.'
      );
    }
  }

  $out = $path . '/data';
  tripal_jbrowse_mgmt_safe_exec(
    'perl',
    [
      $bin . '/prepare-refseqs.pl',
      '--fasta',
      $instance->file,
      '--out',
      $out,
    ],
    $ignore,
    $ret
  );

  return $ret;
}

/**
 * Add a track to an instance.
 *
 * @param $track
 *
 * @return string
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_cmd_add_track($track) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];
  $instance = $track->instance;
  $menu_template = $settings['menu_template'];

  $path = $data . '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $path = $path . '_' . $instance->analysis_id;
  }
  $out = $path . '/data';

  if (!file_exists($out)) {
    throw new Exception('Data directory does not exist: ' . $out);
  }

  switch ($track->track_type)
  {
    case 'HTMLVariants':
      $json = tripal_jbrowse_mgmt_get_json($instance);
      $directory = 'vcf';
      $file_name = $track->file;
      if (is_dir($track->file)) {
        $file_name = glob($track->file . '/' . '*.vcf.gz')[0];
        $index_name = glob($track->file . '/' . '*.vcf.gz.[tci][bsd][ix]')[0];
      }
      $file_name = pathinfo($file_name)['basename'];

      $track_in_json = [
        'label' => tripal_jbrowse_mgmt_make_slug($track->label),
        'key' => $track->label,
        'urlTemplate' => $directory . '/' . $file_name,
        'type' => $track->track_type,
      ];
      // check if index format and give specific (csi)urltemplate / storeClass
      if ($index_name){
        $index_name = pathinfo($index_name)['basename'];
        $extension = pathinfo($index_name)['extension'];
        switch ($extension)
        {
          case 'csi':
            $track_in_json['storeClass'] = 'JBrowse/Store/SeqFeature/VCFTabix';
            $track_in_json['csiUrlTemplate'] = $directory . '/' . $index_name;
            break;
          case 'tbi':
            $track_in_json['storeClass'] = 'JBrowse/Store/SeqFeature/VCFTabix';
            break;
          case 'idx':
            $track_in_json['storeClass'] = 'JBrowse/Store/SeqFeature/VCFTribble';
            break;
        }
      }

      $json['tracks'][] = $track_in_json;
      tripal_jbrowse_mgmt_save_json($instance, $json);
      break;

    case 'Alignment':
      $json = tripal_jbrowse_mgmt_get_json($instance);
      $directory = 'bam';
      $file_name = $track->file;
      if (is_dir($track->file)) {
        $file_name = glob($track->file . '/' . '*.[bam][cram]')[0];
      }
      $file_name = pathinfo($file_name)['basename'];

      $track_in_json = [
        'label' => tripal_jbrowse_mgmt_make_slug($track->label),
        'key' => $track->label,
        'urlTemplate' => $directory . '/' . $file_name,
        'type' => 'JBrowse/View/Track/Alignment2',
      ];

      $extension = pathinfo($file_name)['extension'];
      switch($extension){
        case 'bam':
          $track_in_json['storeClass'] = 'JBrowse/Store/SeqFeature/BAM';
          break;
        case 'cram':
          $track_in_json['storeClass'] = 'JBrowse/Store/SeqFeature/CRAM';
          break;
      }
      tripal_jbrowse_mgmt_save_json($instance, $json);
      break;

    case 'XYPlot':
      $json = tripal_jbrowse_mgmt_get_json($instance);
      $basename = pathinfo($track->file)['basename'];

      $json['tracks'][] = [
        'label' => tripal_jbrowse_mgmt_make_slug($track->label),
        'key' => $track->label,
        'storeClass' => 'JBrowse/Store/SeqFeature/BigWig',
        'urlTemplate' => 'wig/' . $basename,
        'type' => 'JBrowse/View/Track/Wiggle/XYPlot',
      ];

      tripal_jbrowse_mgmt_save_json($instance, $json);
      break;

    default:
    tripal_jbrowse_mgmt_safe_exec(
      'perl',
      [
        $bin . '/flatfile-to-json.pl',
        '--' . $track->file_type,
        $track->file,
        '--trackLabel',
        tripal_jbrowse_mgmt_make_slug($track->label),
        '--key',
        $track->label,
        '--out',
        $out,
        '--trackType',
        $track->track_type,
      ],
      $ignore,
      $ret
    );
    return $ret;
  }
}

/**
 * Generate names for a specific instance.
 *
 * @param $instance
 *
 * @return string
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_cmd_generate_names($instance) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];

  $path = $data . '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $path = $path . '_' . $instance->analysis_id;
  }

  if (!file_exists($path)) {
    if (!mkdir($path)) {
      throw new Exception(
        'Unable to make data directory! Please make sure the directory
      at ' . $data . ' exists and is writable by the current user.'
      );
    }
  }

  $out = $path . '/data';
  tripal_jbrowse_mgmt_safe_exec(
    'perl',
    [
      $bin . '/generate-names.pl',
      '--out',
      $out,
    ],
    $ignore,
    $ret
  );

  return $ret;
}

/**
 * Delete a track to an instance.
 *
 * @param $track
 *
 * @return string
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_cmd_delete_track($track) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];
  $instance = $track->instance;

  $path = $data . '/' . tripal_jbrowse_mgmt_make_slug($instance->title);
  if(isset($instance->analysis_id)){
    $path = $path . '_' . $instance->analysis_id;
  }

  $out = $path . '/data';

  if (!file_exists($out)) {
    throw new Exception('Data directory does not exist: ' . $out);
  }

  return tripal_jbrowse_mgmt_safe_exec(
    'perl',
    [
      $bin . '/remove-track.pl',
      '--trackLabel',
      tripal_jbrowse_mgmt_make_slug($track->label),
      '--dir',
      $out,
      '--delete',
    ]
  );
}

/**
 * Safely execute a command.
 *
 * @param string $command The path to the command to execute.
 * @param array $args Arguments passed as flag => $argument or a list of
 *   arguments as [arg1, arg2, arg3]
 * @param array $output If the output argument is present, then the
 * specified array will be filled with every line of output from the
 * command. Trailing whitespace, such as \n, is not
 * included in this array. Note that if the array already contains some
 * elements, exec will append to the end of the array.
 * If you do not want the function to append elements, call
 * unset on the array before passing it to
 * exec.
 * @param int $return_var If the return_var argument is present
 * along with the output argument, then the
 * return status of the executed command will be written to this
 *
 * @return string
 */
function tripal_jbrowse_mgmt_safe_exec(
  $command,
  array $args = [],
  &$output = NULL,
  &$return = NULL
) {
  $cmd = escapeshellcmd($command) . ' ';
  foreach ($args as $flag => $arg) {
    if (is_string($flag)) {
      $cmd .= escapeshellarg($flag) . ' ';
    }

    $cmd .= escapeshellarg($arg) . ' ';
  }

  print "Running the following command:\n";
  print $cmd . "\n";

  return exec($cmd, $output, $return);
}
