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

    if (!module_exists('tripal_jbrowse')) {
      $this->markTestSkipped('No need to test if not enabled.');
    }

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

  /**
   * Test Creating a JBrowse Instance Node.
   *
   * Note: We can't test this by submitting the form via PUT because it requires
   *  permission to access /node/add/jbrowse_instance; however, we don't yet have a
   *  way to do this with TripalTestSuite. Furthermore, testing HTTP Requests
   *  would not give us access to the data added via the test due to database
   *  transactions.
   */
  public function testJBrowseInstanceNodeCreate() {
    module_load_include('inc', 'node', 'node.pages');

    if (!module_exists('tripal_jbrowse')) {
      $this->markTestSkipped('No need to test if not enabled.');
    }

    // Log in the god user.
    global $user;
    $user = user_load(1);
    $node = array('type' => 'jbrowse_instance');

    // Fill in the form.
    $faker = Factory::create();
    $form_state = array('values' => array());
    $form_state['values']['title'] = $faker->words(3, true);
    $form_state['values']['field_jburl']['und'][0]['url'] = 'https://jbrowse.org/code/JBrowse-1.15.4/';
    $form_state['values']['field_datadir']['und'][0] = 'sample_data/json/volvox';
    $form_state['values']['field_jbloc']['und'][0] = 'ctgA:1..11000';
    $form_state['values']['field_jbtracks']['und'][0] = 'DNA,Genes,volvox-sorted-vcf,volvox_microarray_bw_density,volvox_bb';
    $form_state['values']['op'] = t('Save');

    // Execute the node creation form.
    drupal_form_submit('jbrowse_instance_node_form', $form_state, (object) $node);

    // Retrieve any errors.
    $errors = form_get_errors();

    // Assert that there must not be any.
    $this->assertEmpty($errors, 'Form submission returned the following errors:'.print_r($errors,TRUE));

    // Check that there is a test jbrowse instance.
    $result = db_query('SELECT * FROM {node} WHERE title=:name',
      array(':name' => $form_state['values']['title']));
    $this->assertEquals(1, $result->rowCount(), 'Unable to select the JBrowse Instance using the name.');

    // log out the god user.
    $user = drupal_anonymous_user();
  }

  /**
   * Update an existing Blast Database Node.
   */
  public function testJBrowseInstanceNodeUpdate() {
    module_load_include('inc', 'node', 'node.pages');

    if (!module_exists('tripal_jbrowse')) {
      $this->markTestSkipped('No need to test if not enabled.');
    }
    
    // Log in the god user.
    global $user;
    $user = user_load(1);

    // Create the node in the first place.
    $seeder = \Tests\DatabaseSeeders\JBrowseInstanceNodeSeeder::seed();
    $node = $seeder->getNode();

    // Now use the form to edit it :-)
    // Specifically, we will change the name, url and data directory.
    $faker = Factory::create();
    $form_state = array('values' => array());
    $form_state['values']['title'] = $faker->words(5, true);
    $form_state['values']['field_jburl']['und'][0]['url'] = 'https://jbrowse.org/code/JBrowse-1.15.4/';
    $form_state['values']['field_datadir']['und'][0] = 'sample_data/json/modencode';
    $form_state['values']['op'] = t('Save');

    // Execute the node creation form.
    drupal_form_submit('jbrowse_instance_node_form', $form_state, $node);

    // Retrieve any errors.
    $errors = form_get_errors();
    // Assert that there must not be any.
    $this->assertEmpty($errors, 'Form submission returned the following errors:'.print_r($errors,TRUE));

    // Check that there is a test jbrowse instance.
    $result = db_query('SELECT * FROM {node} WHERE title=:name',
      array(':name' => $form_state['values']['title']));
    $this->assertEquals(1, $result->rowCount(), 'Unable to select the JBrowse Instance using the name.');

    // log out the god user.
    $user = drupal_anonymous_user();
  }

  /**
   * Test deleting a node.
   * NOTE: We cannot test this via drupal_form_submit() since it requires a confirmation.
   */
}
