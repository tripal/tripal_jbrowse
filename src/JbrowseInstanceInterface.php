<?php

namespace Drupal\tripal_jbrowse;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface defining a jbrowse instance entity type.
 */
interface JbrowseInstanceInterface extends ContentEntityInterface, EntityOwnerInterface {

}
