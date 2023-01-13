<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "sequence_feature_jbrowse_type",
 *   label = @Translation("Sequence Feature JBrowse Type"),
 *   description = @Translation("An embeddable JBrowse on a sequence feature page."),
 *   default_widget = "sequence_feature_jbrowse_widget",
 *   default_formatter = "sequence_feature_jbrowse_formatter"
 * )
 */
class SequenceFeatureJBrowseType extends ChadoFieldItemBase {

  public static $id = "sequence_feature_jbrowse_type";

  /**
  * {@inheritdoc}
  */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
		$settings['termIdSpace'] = 'operation';
		$settings['termAccession'] = '0564';
    return $settings; 
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];
    return $elements + parent::fieldSettingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];
    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();
    return $constraints;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the settings for this field.
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];

    // Determine the primary key of the base table.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Return the array of property types.
    return [

			// Gene ID
      new ChadoIntStoragePropertyType($entity_type_id, self::$id,'feature_id', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),
			
			// Organism ID
			new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'organism_id', [
				'action' => 'store',
				'chado_table' => $base_table,
				'chado_column' => $base_fk_col,
			]),

			// Organism Genus
			new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'genus', $genus_len, [
				'action' => 'join',
				'path' => $base_table . '.organism_id>organism.organism_id',
				'chado_column' => 'genus'
			]),

			// Organism Species
			new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'species', $species_len, [
				'action' => 'join',
				'path' => $base_table . '.organism_id>organism.organism_id',
				'chado_column' => 'species'
			]),

			// Gene Name
			new ChadoIntStoragePropertyType($entity_type_id, self::$id,'feature_name', [
        'action' => 'join',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),

			// Genome Assembly

			// featureloc ID

			// Sequence start: featureloc fmin

			// Sequence end: featureloc fmax

			// srcfeature ID (ie. chromosome or scaffold)

			// 

			
    ];
  }
}