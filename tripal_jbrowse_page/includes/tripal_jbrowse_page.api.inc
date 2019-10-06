<?php
/**
 * @file
 * Contains Application Programmer Interface (API) functions for this module.
 */

/**
 * Retrieve the instance id based on the organism.
 */
function tripal_jbrowse_page_get_instance_id($conditions, $options) {

  // First retrieve the organism_id.
  $organism_id = chado_query('SELECT organism_id FROM {organism} WHERE genus=:genus AND species=:species',
    [
      ':genus' => $conditions['genus'],
      ':species' => $conditions['species']
    ])->fetchField();

  // Then retrieve the instance for that organism.
  if ($options['load_instance']) {
    $instances = tripal_jbrowse_mgmt_get_instances(['organism_id' => $organism_id]);
    return $instances[0];
  }
  else {
    return db_select('tripal_jbrowse_mgmt_instances', 'I')
      ->fields('I', ['id'])
      ->condition('organism_id', $organism_id)
      ->execute()->fetchField();
  }

}
