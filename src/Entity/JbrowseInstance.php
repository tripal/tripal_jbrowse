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
 *   label_singular = @Translation("jbrowse instance"),
 *   label_plural = @Translation("jbrowse instances"),
 *   label_count = @PluralTranslation(
 *     singular = "@count jbrowse instances",
 *     plural = "@count jbrowse instances",
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
      ->setLabel(t('Label'))
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
      ->setDisplayOptions('form', [
        'type' => 'text_textarea',
        'weight' => 10,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayOptions('view', [
        'type' => 'text_default',
        'label' => 'above',
        'weight' => 10,
      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setTranslatable(TRUE)
      ->setLabel(t('Author'))
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
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'author',
        'weight' => 15,
      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Authored on'))
      ->setTranslatable(TRUE)
      ->setDescription(t('The time that the jbrowse instance was created.'))
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'timestamp',
        'weight' => 20,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayOptions('form', [
        'type' => 'datetime_timestamp',
        'weight' => 20,
      ])
      ->setDisplayConfigurable('view', TRUE);

    return $fields;
  }

}
