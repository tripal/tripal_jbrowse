<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;
use Faker\Factory;

class JBrowseInstanceNodeSeeder extends Seeder
{
    /**
     * Seeds the database with users.
     *
     * @return void
     */
    public function up()
    {
      $faker = Factory::create();

      // Log in the god user.
      global $user;
      $user = user_load(1);
      $node = new \stdClass();

      if (!isset($node->title)) $node->title = $faker->name();

      $node->type = 'jbrowse_instance';
      node_object_prepare($node);

      $node->language = LANGUAGE_NONE;
      $node->uid = $user->uid;
      $node->status = 1;  // published.
      $node->promote = 0; // not promoted.
      $node->comment = 0; // disabled.

      $node->field_jburl['und'][0]['url'] = $faker->url();
      $node->field_datadir['und'][0] = 'fake/path';
      $node->field_jbloc['und'][0] = $faker->word() .':'. rand(0,1000).'..'.rand(2000, 10000);
      $node->field_jbtracks['und'][0] = str_replace(' ',',',$faker->words(5,true));

      $node = node_submit($node);
      node_save($node);

      // log out the god user.
      $user = drupal_anonymous_user();
      $this->node = $node;

    }

    /**
     * Returns the node created by up().
     */
     public function getNode() {
       return $this->node;
     }
}
