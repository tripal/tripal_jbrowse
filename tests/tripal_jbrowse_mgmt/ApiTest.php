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
   */
  public function testSettings() {

    $test_settings = [
      'bin_path' => 'test/fake/path',
	    'link' => 'test/fake/path',
	    'data_dir' => 'test/fake/path',
	    'menu_template' => [],
    ];

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

  /**
   * Test Instance Create-Retrieve-Update-Delete.
   */
  public function testInstanceCRUD() {
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");

    $organism = factory('chado.organism')->create();

    // Full Fake Instance Details.
    $faker = Factory::create();
    $testdata = [
      'organism_id' => $organism->organism_id,
      'title' => $faker->words(3, TRUE),
      'description' => $faker->sentence(25, TRUE),
      'created_at' => $faker->unixTime(),
      'file' => '/path/to/fake/file',
    ];

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

    // and delete with numeric id.
    $success = tripal_jbrowse_mgmt_delete_instance($id);
    $this->assertNotFalse($success, 'Unable to delete this instance.');
    // and catch an exception during delete.
    try {
      $success = tripal_jbrowse_mgmt_delete_instance('FAKESTRING');
      // If we get here without an exception then this test should fail.
      $this->assertTrue(FALSE, "Shouldn't be able to extract instance to delete.");
    } catch (\Exception $e) {
        // Not worried since this is expected!
    }

    putenv("TRIPAL_SUPPRESS_ERRORS");
  }

  /**
   * Test Track Create-Retrieve-Update-Delete.
   */
  public function testTrackCRUD() {

    // Fake Instance.
    $faker = Factory::create();
    $organism = factory('chado.organism')->create();
    $instance_details = [
      'organism_id' => $organism->organism_id,
      'title' => $faker->words(3, TRUE),
      'description' => $faker->sentence(25, TRUE),
      'created_at' => $faker->unixTime(),
      'file' => '/path/to/fake/file',
    ];
    $instance_id = tripal_jbrowse_mgmt_create_instance($instance_details);
    $instance = tripal_jbrowse_mgmt_get_instance($instance_id);

    $path = tripal_jbrowse_mgmt_get_track_list_file_path($instance);
    $this->assertNotFalse($path,
      "Unable to retrieve the path to the trackList.json");

    // Fake track details.
    $testdata = [
      'label' => $faker->words(2, TRUE),
      'track_type' => 'CanvasFeatures',
      'file_type' => 'gff',
      'created_at' => $faker->unixTime(),
      'file' => '/path/to/fake/file',
    ];

    // First retrieve when there are no tracks.
    $tracks = tripal_jbrowse_mgmt_get_tracks($instance);
    $this->assertCount(0, $tracks,
      "There should not be tracks as we just created this instance.");

    // Create tracks.
    $track_id = tripal_jbrowse_mgmt_create_track($instance, $testdata);
    $this->assertNotFalse($track_id,
      "We should have a track created successfully.");

    // Retrieve our newly created track.
    $track = tripal_jbrowse_mgmt_get_track($track_id);
    $this->assertNotFalse($track, "Unable to create track.");
    $this->assertEquals($testdata['label'], $track->label,
      "The label of our new track didn't match what we submitted.");
    $this->assertEquals($instance->id, $track->instance_id,
      "The instance_id of our new track didn't match what we submitted.");
    $this->assertEquals($instance->organism_id, $track->organism_id,
      "The organism_id of our new track didn't match what we submitted.");

    // Retrieve track with a condition.
    $tracks = tripal_jbrowse_mgmt_get_tracks($instance, ['label' => $testdata['label']]);
    $this->assertCount(1, $tracks, "We were not able to select a track we knew should exist.");

    // Now update it.
    $new_label = 'NEW LABEL ' . uniqid();
    $success = tripal_jbrowse_mgmt_update_track($track, ['label' => $new_label]);
    $this->assertNotFalse($success, "Unable to update track label.");
    $new_track = tripal_jbrowse_mgmt_get_track($track_id);
    $this->assertEquals($new_label, $new_track->label,
      "The label of the track was not updated.");

    // Finally, delete it.
    $success = tripal_jbrowse_mgmt_delete_track($track_id);
    $this->assertNotFalse($success, "Unable to delete track.");
    $success = tripal_jbrowse_mgmt_delete_track($track_id);
    $this->assertEquals(0, $success, "Deleted a track that doesn't exist?");
  }

  /**
   * Test organism-related api functions.
   */
  public function testOrganismAPI() {

    // Fake Organism.
    $faker = Factory::create();
    $organism = factory('chado.organism')->create();

    $organism_list = tripal_jbrowse_mgmt_get_organisms_list();
    $this->assertNotCount(0, $organism_list, "There should be at least one organism.");
    $this->assertArrayContainsObjectValue($organism_list, 'organism_id', $organism->organism_id);

    $organism_name = tripal_jbrowse_mgmt_construct_organism_name($organism);
    $this->assertRegexp('/'.$organism->genus.'/', $organism_name,
      "The organism name did not contain the genus.");
    $this->assertRegexp('/'.$organism->species.'/', $organism_name,
      "The organism name did not contain the species.");

    $slug = tripal_jbrowse_mgmt_make_slug($organism_name);
    $this->assertNotRegexp('/ /', $slug,
      "The organism slug should not contain spaces.");
  }

  /**
   * Test Instance Properties Create-Retrieve-Update.
   */
  public function testInstanceProperyCRU() {

    // Fake Instance.
    $faker = Factory::create();
    $organism = factory('chado.organism')->create();
    $instance_details = [
      'organism_id' => $organism->organism_id,
      'title' => $faker->words(3, TRUE),
      'description' => $faker->sentence(25, TRUE),
      'created_at' => $faker->unixTime(),
      'file' => '/path/to/fake/file',
    ];
    $instance_id = tripal_jbrowse_mgmt_create_instance($instance_details);

    $testdata = [
      $faker->word() => $faker->words(3, TRUE),
      $faker->word() => $faker->words(2, TRUE),
      $faker->word() => $faker->words(10, TRUE),
    ];

    // Create.
    tripal_jbrowse_mgmt_save_instance_properties($instance_id, $testdata);

    // Retrieve.
    $properties = tripal_jbrowse_mgmt_get_instance_properties($instance_id);
    $this->assertIsArray($properties, "Unable to retrieve newly created properties.");
    $this->assertCount(3, $properties,
      "There was not the expected count of properties.");

    // Update single property.
    $property_name = key($testdata);
    $new_value = 'NEW VALUE '.uniqid();
    tripal_jbrowse_mgmt_save_instance_property($instance_id, $property_name, $new_value);
    $retrieved_value = tripal_jbrowse_mgmt_get_instance_property($instance_id, $property_name);
    $this->assertEquals($new_value, $retrieved_value,
      "We were unable to update a single property.");

  }

  /**
   * Test Miscellaneous API functions.
   */
  public function testMiscAPI() {

    $track_types = tripal_jbrowse_mgmt_get_track_types();
    $this->assertIsArray($track_types,
      "The track types should be an array.");

  }
  /**
   * Provide an assertion to check properties of an array of objects.
   *
   * For example, if you have an array of organism objects, the following code
   * would check that one object in the array has an organism_id of 50.
   * @code $this->assertArrayContainsObjectValue($array, 'organism_id', 50);
   */
  private function assertArrayContainsObjectValue($theArray, $attribute, $value)
  {
      foreach($theArray as $arrayItem) {
          if($arrayItem->$attribute == $value) {
              return true;
          }
      }
      return false;
  }
}
