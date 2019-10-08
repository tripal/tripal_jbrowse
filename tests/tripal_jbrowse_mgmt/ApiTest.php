<?php
namespace Tests\tripal_jbrowse_mgmt;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class ApiTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Tests the Settings API.
   *
   * Specifically tripal_jbrowse_mgmt_save_settings()
   * and tripal_jbrowse_mgmt_get_settings().
   *
   * @dataProvider settingsProvider
   */
  public function testSettings($test_settings) {

    tripal_jbrowse_mgmt_save_settings($test_settings);
    $retrieved = tripal_jbrowse_mgmt_get_settings();

    // Check that all the expected settings are available.
    $expected = ['bin_path', 'link', 'data_dir', 'menu_template'];
    foreach ($expected as $expected_key) {
      $this->assertArrayHasKey($expected_key, $retrieved,
        "Retrieved settings do not contain $expected_key");

      // Check that the settings match if provided in test.
      if (isset($test_settings[$expected_key])) {
        $this->assertEquals(
          $test_settings[$expected_key],
          $retrieved[$expected_key],
          "The retrieved value for $expected_key does not match what we set."
        );
      }
    }
  }
  // Associated Data Provider for testing settings.
  public function settingsProvider() {
    $faker = Factory::create();
    $sets = [];

    // No settings.
    $sets[] = [[]];

    // Full Fake Settings.
    $sets[] = [[
      'bin_path' => 'test/fake/path',
	    'link' => 'test/fake/path',
	    'data_dir' => 'test/fake/path',
	    'menu_template' => [],
    ]];

    return $sets;
  }

  /**
   * Test Instance Create-Retrieve-Update-Delete.
   *
   * @dataProvider instanceProvider
   */
  public function testInstanceCRUD($testdata) {
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");

    // Check we cannot create a JBrowse instance without an organism.
    $noOrganism = $testdata;
    unset($noOrganism['organism_id']);
    $id = tripal_jbrowse_mgmt_create_instance($noOrganism);
    $this->assertFalse($id,
      "Created an instance without an organism_id!?!");

    // Now try to create an instance with all the data.
    $id = tripal_jbrowse_mgmt_create_instance($testdata);
    $this->assertNotFalse($id,
      "Unable to create instance.");

    // Try to retrieve the instance we just created.
    $retrieved_instance = tripal_jbrowse_mgmt_get_instance($id);
    $this->assertNotFalse($id, "We did not retrieve an instance?");
    $this->assertEquals($id, $retrieved_instance->id,
      "We retrieved a different instance then we asked for?");
    $this->assertEquals(
      $testdata['organism_id'], $retrieved_instance->organism_id,
      "Retreived the same instance but the organism is not correct?");

    // Change the title and test the instance was updated.
    $new_title = $testdata;
    $testdata['title'] = 'NEW FAKE TITLE ' . uniqid();
    $success = tripal_jbrowse_mgmt_update_instance($id, $new_title);
    $this->assertNotFalse($success, 'Unable to update instance title.');
    $retrieved_instance = tripal_jbrowse_mgmt_get_instance($id);
    $this->assertEquals(
      $new_title['title'], $retrieved_instance->title,
      "The title was not updated.");

    // Finally delete him!
    $success = tripal_jbrowse_mgmt_delete_instance($retrieved_instance);
    $this->assertNotFalse($success, 'Unable to delete this instance.');
    $one_more_time = tripal_jbrowse_mgmt_get_instance($id);
    $this->assertFalse($one_more_time);

    putenv("TRIPAL_SUPPRESS_ERRORS");
  }
  // Associated Data Provider for testing instances.
  public function instanceProvider() {
    $faker = Factory::create();
    $sets = [];

    $organism = factory('chado.organism')->create();

    // Full Fake Instance Details.
    $sets[] = [[
      'organism_id' => $organism->organism_id,
      'title' => $faker->words(3, TRUE),
      'description' => $faker->sentence(25, TRUE),
      'created_at' => $faker->unixTime(),
      'file' => '/path/to/fake/file',
    ]];

    return $sets;
  }

}
