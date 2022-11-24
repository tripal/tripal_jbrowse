<?php

namespace Drupal\tripal_jbrowse_mgmt\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerTrait;

class TripalJBrowseMgmtRegisterForm implements FormInterface{
	
	use MessengerTrait;

	/**
	 * Form ID
	 * 
	 * @return string
	 */
	public function getFormId() {
		return 'tripal_jbrowse_mgmt_register_form';
	}

	/**
	 * Allow users to register an existing JBrowse instance.
	 * 
	 * @param array $form
	 * @param \Drupal\Core\Form\FormStateInterface $form_state
	 * 
	 * @return array
	 */
	public function buildForm(array $form, FormStateInterface $form_state) {

    // Get the list of organisms.
    $organisms = [];
    $chado = \Drupal::service('tripal_chado.database');
		// Start building the query to pull organisms from Chado
    $query = $chado->select('organism', 'o');
    $query->leftJoin('cvterm', 'cvt', 'o.type_id = cvt.cvterm_id');
    $query->fields('o', ['organism_id', 'genus', 'species', 'infraspecific_name']);
    $query->fields('cvt', ['name']);
    $query->orderBy('genus');
    $query->orderBy('species');
    $results = $query->execute();
		// Iterating through the results of our query to build our dropdown
    while ($organism = $results->fetchObject()) {
      $org_name = $organism->genus . ' ' . $organism->species;

      if ($organism->name) {
        $org_name .= ' ' . $organism->name;
      }
      if ($organism->infraspecific_name) {
        $org_name .= ' ' . $organism->infraspecific_name;
      }
      $organisms[$organism->organism_id] = $org_name;
    }

		$form_description = 'Create a new JBrowse instance for a given organism. Submitting this form
		creates all the necessary files for a new JBrowse instance.';
		$form['description_of_form'] = [
			'#type' => 'item',
			'#markup' => $form_description,
		];

		$form['title'] = [
			'#title' => 'Instance Name',
			'#description' => 'Choose a human-readable name for this JBrowse instance.',
			'#type' => 'textfield',
			'#required' => TRUE,
		];

		$form['organism'] = [
			'#title' => 'Organism',
			'#description' => 'Select the organism',
			'#type' => 'select',
			'#options' => $organisms,
			'#required' => TRUE,
		];

	/*  $analysis_term_entity = tripal_load_term_entity([
			'vocabulary'=>'operation',
			'accession'=>'2945',
		]);

		$analysis_bundle_entity = tripal_load_bundle_entity([
			'term_id'=>$analysis_term_entity->id,
		]);
	*/
		$form['description'] = [
			'#title' => 'Description',
			'#description' => 'Optional description for the instance.',
			'#type' => 'textarea',
			'#required' => TRUE,
		];

/*		$form['analysis'] = [
			'#title' => 'Sequence Assembly',
			'#description' => 'Select the analysis which describes the sequence assembly used as the backbone for this JBrowse instance. An analysis can be created in (ADD LINK TO ANALYSIS PAGE HERE) if it is not already available.<br><strong>Please choose analysis carefully</strong> since it can not change once instance is created.',
			'#type' => 'textfield',
	//    '#autocomplete_path' => 'admin/tripal/extension/tripal_jbrowse/management/instances/analysis/autocomplete', 
		];
*/
		$form['jbrowse_url'] = [
			'#title' => 'JBrowse URL',
			'#description' => 'Enter the URL of an exisiting JBrowse instance that you want to register.',
			'#type' => 'url',
			'#required' => TRUE,
		];
	/*
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
	*/
		$form['submit'] = [
			'#type' => 'submit',
			'#value' => 'Register Instance',
		];

		return $form;
	}

	/**
   * Validate form.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {

	}
	
	
	
	/**
	 * Save settings.
	 *
	 * @param array $form
	 * @param \Drupal\Core\Form\FormStateInterface $form_state
	 */
	public function submitForm(array &$form, FormStateInterface $form_state) {

		$values = [];
		$values['title'] = $form_state->getValue('title');
		$values['organism_id'] = $form_state->getValue('organism');
		$values['uid'] = \Drupal::currentUser()->id();
		$values['description'] = $form_state->getValue('description');
/*		if (!empty($form_state->getValue('analysis'))) {
			$values['analysis_id'] = $form_state->getValue('analysis');
		} */
		$values['created_at'] = \Drupal::time()->getRequestTime();
		$values['url'] = $form_state->getValue('jbrowse_url');

		$connection = \Drupal::service('database');

		$result = $connection->insert('tripal_jbrowse_mgmt_instances')
			->fields($values)
			->execute();
		
			$this->messenger()->addStatus('JBrowse instance successfully registered.');
	}
}


	