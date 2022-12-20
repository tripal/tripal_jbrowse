<?php

namespace Drupal\tripal_jbrowse\Entity;

use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\tripal_jbrowse\JbrowseInstanceInterface;
use Drupal\user\EntityOwnerTrait;

/**
 * Defines the jbrowse instance entity class.
 *
 * @ContentEntityType(
 *   id = "jbrowse_instance",
 *   label = @Translation("JBrowse Instance"),
 *   label_collection = @Translation("JBrowse Instances"),
 *   label_singular = @Translation("JBrowse instance"),
 *   label_plural = @Translation("JBrowse instances"),
 *   label_count = @PluralTranslation(
 *     singular = "@count JBrowse instances",
 *     plural = "@count JBrowse instances",
 *   ),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal_jbrowse\JbrowseInstanceListBuilder",
 *     "views_data" = "Drupal\views\EntityViewsData",
 *     "access" = "Drupal\tripal_jbrowse\JbrowseInstanceAccessControlHandler",
 *     "form" = {
 *       "add" = "Drupal\tripal_jbrowse\Form\JbrowseInstanceForm",
 *       "edit" = "Drupal\tripal_jbrowse\Form\JbrowseInstanceForm",
 *       "delete" = "Drupal\Core\Entity\ContentEntityDeleteForm",
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\Core\Entity\Routing\AdminHtmlRouteProvider",
 *     }
 *   },
 *   base_table = "jbrowse_instance",
 *   data_table = "jbrowse_instance_field_data",
 *   translatable = TRUE,
 *   admin_permission = "administer jbrowse instance",
 *   entity_keys = {
 *     "id" = "id",
 *     "langcode" = "langcode",
 *     "label" = "label",
 *     "uuid" = "uuid",
 *     "owner" = "uid",
 *     "organism_page_id" = "organism_page_id",
 *     "assembly_page_id" = "assembly_page_id",
 *     "jbrowse_url" = "jbrowse_url",
 *     "jbrowse_version" = "jbrowse_version",
 *   },
 *   links = {
 *     "collection" = "/admin/content/jbrowse-instance",
 *     "add-form" = "/jbrowse/add",
 *     "canonical" = "/jbrowse/{jbrowse_instance}",
 *     "edit-form" = "/jbrowse/{jbrowse_instance}/edit",
 *     "delete-form" = "/jbrowse/{jbrowse_instance}/delete",
 *   },
 *   field_ui_base_route = "entity.jbrowse_instance.settings",
 * )
 */
class JbrowseInstance extends ContentEntityBase implements JbrowseInstanceInterface {

  use EntityOwnerTrait;

  /**
   * {@inheritdoc}
   */
  public function preSave(EntityStorageInterface $storage) {
    parent::preSave($storage);
    if (!$this->getOwnerId()) {
      // If no owner has been set explicitly, make the anonymous user the owner.
      $this->setOwnerId(0);
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {

    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['label'] = BaseFieldDefinition::create('string')
      ->setTranslatable(TRUE)
      ->setLabel(t('Title'))
      ->setDescription(t('Choose a human-readable name for this JBrowse instance.'))
      ->setRequired(TRUE)
      ->setSetting('max_length', 255)
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -5,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayOptions('view', [
        'label' => 'hidden',
        'type' => 'string',
        'weight' => -5,
      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['description'] = BaseFieldDefinition::create('text_long')
      ->setTranslatable(TRUE)
      ->setLabel(t('Description'))
      ->setDescription(t('Optional description for the instance.'))
      ->setDisplayOptions('form', [
        'type' => 'text_textarea',
        'weight' => -5,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayOptions('view', [
        'type' => 'text_default',
        'label' => 'hidden',
        'weight' => 10,
      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setTranslatable(TRUE)
      ->setLabel(t('Author'))
      ->setDescription(t('The user registrating this JBrowse instance.'))
      ->setSetting('target_type', 'user')
      ->setDefaultValueCallback(static::class . '::getDefaultEntityOwner')
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => 60,
          'placeholder' => '',
        ],
        'weight' => 15,
      ])
      ->setDisplayConfigurable('form', TRUE)
//      ->setDisplayOptions('view', [
//        'label' => 'above',
//        'type' => 'author',
//        'weight' => 15,
//      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Authored on'))
      ->setTranslatable(TRUE)
      ->setDescription(t('The time that the JBrowse instance was created.'))
//      ->setDisplayOptions('view', [
//        'label' => 'above',
//        'type' => 'hidden',
//        'weight' => 20,
//      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayOptions('form', [
        'type' => 'datetime_timestamp',
        'weight' => 20,
      ])
      ->setDisplayConfigurable('view', TRUE);

    //@TODO: Replace bio_data_1 with configuration variable setting  
    $fields['organism_page_id'] = BaseFieldDefinition::create('entity_reference')
      ->setTranslatable(TRUE)
      ->setLabel(t('Organism'))
      ->setDescription(t('Select the organism.'))
      ->setRequired(TRUE)
      ->setSetting('target_type', 'tripal_entity')
      ->setSetting('handler_settings', ['target_bundles' => ['bio_data_1' => 'bio_data_1']])
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => 60,
          'placeholder' => '',
        ],
        'weight' => 10,
      ])
      ->setDisplayConfigurable('form', TRUE)
//      ->setDisplayOptions('view', [
//        'label' => 'inline',
//        'type' => 'tripal_entity',
//        'weight' => 10,
//      ])
      ->setDisplayConfigurable('view', TRUE);

    //@TODO: Replace bio_data_13 with configuration variable setting  
    $fields['assembly_page_id'] = BaseFieldDefinition::create('entity_reference')
      ->setTranslatable(TRUE)
      ->setLabel(t('Genome Assembly'))
      ->setDescription(t('Select the analysis which describes the sequence assembly used as the backbone for this JBrowse instance. <br> <strong>Please choose analysis carefully</strong> since it can not change once instance is created.'))
      ->setRequired(TRUE)
      ->setSetting('target_type', 'tripal_entity')
      ->setSetting('handler_settings', ['target_bundles' => ['bio_data_13' => 'bio_data_13']])
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => 60,
          'placeholder' => '',
        ],
        'weight' => 10,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayOptions('view', [
        'label' => 'inline',
        'type' => 'tripal_entity',
        'weight' => 5,
      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['jbrowse_url'] = BaseFieldDefinition::create('uri')
      ->setTranslatable(TRUE)
      ->setLabel(t('JBrowse URL'))
      ->setDescription(t('Specify the URL of an exisiting JBrowse instance that you want to register.'))
      ->setRequired(TRUE)
      ->setSetting('max_length', 255)
      ->setDisplayOptions('form', [
        'type' => 'url',
        'weight' => -5,
      ])
      ->setDisplayConfigurable('form', TRUE)
//      ->setDisplayOptions('view', [
//        'label' => 'hidden',
//        'type' => 'string',
//        'weight' => -5,
//      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['jbrowse_version'] = BaseFieldDefinition::create('list_string')
      ->setTranslatable(TRUE)
      ->setLabel(t('JBrowse Version'))
      ->setDescription(t('Select which JBrowse version to use for this instance.'))
      ->setRequired(TRUE)
      ->setSettings([
        'allowed_values' => ['1' => '1.x', '2' => '2.x']
      ])
      ->setDefaultValue('2')
      ->setDisplayOptions('form', [
        'type' => 'options_select',
        'weight' => -5,
      ])
      ->setDisplayConfigurable('form', TRUE)
//      ->setDisplayOptions('view', [
//        'label' => 'hidden',
//        'type' => 'string',
//        'weight' => -5,
//      ])
      ->setDisplayConfigurable('view', TRUE);

    return $fields;
  }
}
