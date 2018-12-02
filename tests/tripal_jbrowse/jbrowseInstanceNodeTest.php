<?php
namespace Tests\tripal_jbrowse;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

/**
 * Tests the JBrowse Instance Node Type.
 */
class jbrowseInstanceNodeTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * JBrowse Instance Node Type exists.
   *
   * Check that the JBrowse Instance node type exists. It should be created
   * when the module is installed by the Drupal Node API.
   */
  public function testJbrowseInstanceNodeTypeExists() {

    // Get a list of all types available.
    $types = node_type_get_types();

    // The JBrowse Instance node type must be in the list.
    $this->assertArrayHasKey('jbrowse_instance', $types, '"JBrowse Instance" node type is not registered with Drupal.');

    // Check that the expected fields exist and are attached to the JBrowse instance node type.
    // First retrieve all fields for this node type.
    $fields = field_info_instances('node', 'jbrowse_instance');
    // Now check that those important to us, exist.
    $this->assertArrayHasKey('field_jburl', $fields,
      'The "Existing JBrowse URL" field is not attached to the JBrowse Instance node type.');
    $this->assertArrayHasKey('field_datadir', $fields,
      'The "Data Directory" field is not attached to the JBrowse Instance node type.');
    $this->assertArrayHasKey('field_jbloc', $fields,
      'The "Start Location" field is not attached to the JBrowse Instance node type.');
    $this->assertArrayHasKey('field_jbtracks', $fields,
      'The "Tracks to Display" field is not attached to the JBrowse Instance node type.');
  }


}
