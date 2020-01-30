<?php

function tripal_jbrowse_mgmt_add_form($form, &$form_state) {
  $settings = tripal_jbrowse_mgmt_get_settings();

  // Ensure the module is configured.
  if (empty(array_filter($settings))) {
    $form['incomplete'] = [
      '#type' => 'item',
      '#prefix' => '<div class="messages error">',
      '#markup' => t(
        'You have not configured the module yet. Please visit the
            <a href="@url">settings page</a> and submit the form before continuing.',
        ['@url' => url('admin/tripal/extension/tripal_jbrowse/management/configure')]
      ),
      '#suffix' => '</div>',
    ];

    return $form;
  }

  $organisms = tripal_jbrowse_mgmt_get_organisms_list();
  $mapped_organisms = [];
  foreach ($organisms as $organism) {
    $mapped_organisms[$organism->organism_id] = tripal_jbrowse_mgmt_construct_organism_name(
      $organism
    );
  }

  $form_description = 'Create a new JBrowse instance for a given organism. Submitting this form
  creates all the necessary files for a new JBrowse instance.';
  $form['description_of_form'] = [
    '#type' => 'item',
    '#markup' => t($form_description),
    '#weight' => -10,
  ];

  $form['organism'] = [
    '#title' => t('Organism'),
    '#description' => t('Select the organism'),
    '#type' => 'select',
    '#options' => $mapped_organisms,
    '#required' => TRUE,
    '#weight' => -8,
  ];

  $form['analysis'] = [
    '#title' => t('Analysis'),
    '#description' => 'Select the analysis to which this instance will be related. Analysis can be created in '.l('Add Tripal Content', 'bio_data/add/3').' if wanted analysis is not available.<br><strong>Please choose analysis carefully</strong> since it can not change once instance is created.',
    '#type' => 'textfield',
    '#autocomplete_path' => 'admin/tripal/extension/tripal_jbrowse/management/instances/analysis/autocomplete',
    '#weight' => -6,
  ];

  $form['description'] = [
    '#title' => t('Description'),
    '#description' => t('Optional description for the instance.'),
    '#type' => 'textarea',
    '#weight' => -7,
  ];

  $form['data'] = [
    '#type' => 'fieldset',
    '#title' => t('Reference Sequence File'),
    '#collabsible' => FALSE,
    '#weight' => 0
  ];

  $form['data']['data_desc'] = [
    '#type' => 'item',
    '#markup' => t(
      'You may either upload a file below or provide
       the path to the reference sequence fasta file.'
    ),
  ];

  $form['data']['ref_seq_file'] = [
    '#type' => 'file',
    '#title' => t('Reference Sequence FASTA File'),
  ];

  $form['data']['ref_seq_path'] = [
    '#type' => 'textfield',
    '#title' => t('OR Path to File on Server'),
    '#description' => t(
      'This path will be ignored if a file is provided above. Ex: sites/default/files/file.fasta or /data/file.fasta'
    ),
  ];

  $form['page'] = [
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => 'Instance Page Settings',
    '#description' => 'The following settings pertain to link directing users to this instance (either embedded or the original).',
  ];

  $form['page']['start-loc'] = [
    '#type' => 'textfield',
    '#title' => 'Start Location',
    '#description' => "<p>The initial genomic position which will be visible in
      the viewing field. Possible input strings are:</p>\r\n
    <strong>\"Chromosome\": \"start point\"..\"end point\"</strong>\r\n<p>A
      chromosome name/ID followed by “:”, starting position, “..” and end
      position of the genome to be viewed in the browser is used as an input.
      Chromosome ID can be either a string or a mix of string and numbers.
      “CHR” to indicate chromosome may or may not be used. Strings are not
      case-sensitive. If the chromosome ID is found in the database reference
      sequence (RefSeq), the chromosome will be shown from the starting
      position to the end position given in URL.</p>\r\n
      <pre>     ctgA:100..200</pre>\r\n
      <p>Chromosome ctgA will be displayed from position 100 to 200.</p>\r\n
    OR <strong>start point\"..\"end point</strong>\r\n<p>A string of
      numerical value, “..” and another numerical value is given with the loc
      option. JBrowse navigates through the currently selected chromosome from
      the first numerical value, start point, to the second numerical value,
      end point.</p>\r\n<pre>     200..600</pre>\r\n<p>Displays position 200
      to 600 of the current chromosome.</p>\r\n
    OR <strong>center base</strong>\r\n<p>If only one numerical value is given
      as an input, JBrowse treats the input as the center position. Then an
      arbitrary region of the currently selected gene is displayed in the
      viewing field with the given input position as basepair position on
      which to center the view.</p>\r\n
    OR <strong>feature name/ID</strong>\r\n<p>If a string or a mix of string
      and numbers are entered as an input, JBrowse treats the input as a
      feature name/ID of a gene. If the ID exists in the database RefSeq,
      JBrowser displays an arbitrary region of the feature from the the
      position 0, starting position of the gene, to a certain end point.</p>\r\n
      <pre>     ctgA</pre>\r\n<p>Displays an arbitrary region from the ctgA
      reference.</p>",
  ];

  $form['page']['start-tracks'] = [
    '#type' => 'textarea',
    '#rows' => 2,
    '#title' => 'Tracks to Display',
    '#description' => "<p>A comma-delimited strings containing track names,
      each of which should correspond to the \"label\" element of the track
      information dictionaries that are currently viewed in the viewing field.</p>\r\n
      <pre>     DNA,knownGene,ccdsGene,snp131,pgWatson,simpleRepeat</pre>",
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Create New Instance',
    '#weight' => 10,
  ];

  return $form;
}

/**
 * Validate the form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_jbrowse_mgmt_add_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $organism = isset($values['organism']) ? $values['organism'] : NULL;

  // Check if this is an add or edit form.
  $edit_form = FALSE;
  if (isset($form_state['build_info']['args'][0]) AND is_numeric($form_state['build_info']['args'][0])) {
    $instance_id = $form_state['build_info']['args'][0];
    $edit_form = TRUE;
  }


  $file = $_FILES['files']['tmp_name']['ref_seq_file'];
  $local_file = isset($values['ref_seq_path']) ? $values['ref_seq_path'] : NULL;

  if (empty($file) && empty($local_file)) {
    form_set_error(
      'ref_seq_file',
      'Please provide a local file path or upload a new file.'
    );
  }
  elseif (empty($file) && !empty($local_file)) {
    if (!file_exists($local_file)) {
      form_set_error('ref_seq_path', 'The file path provided does not exist.');
    }
  }
  else {
    $uploaded = tripal_jbrowse_mgmt_upload_file('ref_seq_file');
    if (!$uploaded) {
      form_set_error('ref_seq_file', 'Unable to upload file');
    }
    else {
      $uploaded = tripal_jbrowse_mgmt_move_file($uploaded);
      $form_state['values']['uploaded_file'] = $uploaded;
    }
  }

  //  if selected organism already exist, analysis_id is required
  //  also make sure organism+analysis does not exist
  $instances = tripal_jbrowse_mgmt_get_instances(['organism_id' => $organism]);

  if (empty($values['analysis'])) {
    if (!empty($instances)){
      form_set_error(
        'analysis',
        'A JBrowse instance for the selected organism already exists. Please choose one analysis for this instance.'
      );
    }
  }
  else{
    $values_analysis_id = tripal_jbrowse_mgmt_get_analysis_id_from_string($values['analysis']);
    foreach ($instances as $instance){
      if ($values_analysis_id == $instance->analysis_id){
        form_set_error(
          'analysis',
          'The analysis for selected organism is taken. Please choose another analysis/organism for this instance.'
        );
      }
    }
  }

  $organism = db_select('chado.organism', 'CO')
    ->fields('CO')
    ->condition('organism_id', $organism)
    ->execute()
    ->fetchObject();

  if (empty($organism)) {
    form_set_error('organism', 'Invalid organism selected ' . $organism);
  }
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function tripal_jbrowse_mgmt_add_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];
  $organism_id = $values['organism'];
  if ($values['analysis']){
    $analysis_id = tripal_jbrowse_mgmt_get_analysis_id_from_string($values['analysis']);
  }
  preg_match_all('!\d+!', $values['analysis'], $match_analysis);
  $analysis_id = array_pop($match_analysis[0]);
  $description = isset($values['description']) ? $values['description'] : '';

  if (empty($values['uploaded_file'])) {
    $file = $values['ref_seq_path'];
  }
  else {
    $file = $values['uploaded_file'];
  }

  $organism = db_select('chado.organism', 'CO')
    ->fields('CO')
    ->condition('organism_id', $organism_id)
    ->execute()
    ->fetchObject();

  $instance_id = tripal_jbrowse_mgmt_create_instance(
    [
      'organism_id' => $organism_id,
      'analysis_id' => $analysis_id,
      'title' => tripal_jbrowse_mgmt_construct_organism_name($organism),
      'description' => $description,
      'created_at' => time(),
      'file' => $file,
    ]
  );

  if ($instance_id) {
    drupal_set_message('Instance created successfully!');
    $name = 'Create JBrowse instance for ';
    $name .= tripal_jbrowse_mgmt_construct_organism_name($organism);

    tripal_add_job(
      $name,
      'tripal_jbrowse_mgmt',
      'tripal_jbrowse_mgmt_create_instance_files',
      [$instance_id],
      $user->uid
    );
    $form_state['redirect'] = "admin/tripal/extension/tripal_jbrowse/management/instances/$instance_id";
  }
  else {
    if (empty($values['uploaded_file'])) {
      $file = $values['ref_seq_path'];
    }
    else {
      $file = $values['uploaded_file'];
    }

    $organism = db_select('chado.organism', 'CO')
      ->fields('CO')
      ->condition('organism_id', $organism_id)
      ->execute()
      ->fetchObject();

    $instance_id = tripal_jbrowse_mgmt_create_instance(
      [
        'organism_id' => $organism_id,
        'analysis_id' => $analysis_id,
        'title' => tripal_jbrowse_mgmt_construct_organism_name($organism),
        'description' => $description,
        'created_at' => time(),
        'file' => $file,
      ]
    );

    if ($instance_id) {
      drupal_set_message('Instance created successfully!');
      $name = 'Create JBrowse instance for ';
      $name .= tripal_jbrowse_mgmt_construct_organism_name($organism);

      tripal_add_job(
        $name,
        'tripal_jbrowse_mgmt',
        'tripal_jbrowse_mgmt_create_instance_files',
        [$instance_id],
        $user->uid
      );
      $form_state['redirect'] = "admin/tripal/extension/tripal_jbrowse/management/instances/$instance_id";
    }
    else {
      drupal_set_message('Failed to create instance!', 'error');
    }
  }

  // Now save the instance properties.
  tripal_jbrowse_mgmt_save_instance_properties(
    $instance_id,
    $form_state['values']['page']
  );
}
