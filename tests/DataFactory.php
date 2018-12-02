<?php

use StatonLab\TripalTestSuite\Database\Factory;

/**
 * Data Factory
 * -----------------------------------------------------------
 * This is where you can define factories for use in tests and
 * database seeders.
 *
 * @docs https://github.com/statonlab/TripalTestSuite
 */

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.cv', function (Faker\Generator $faker) {
  return [
    'name' => $faker->unique()->word .  uniqid(),
   // 'name' => $faker->unique($reset = TRUE)->word ,
    'definition' => $faker->text,
  ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.db', function (Faker\Generator $faker) {
  return [
    'name' => $faker->unique()->word .  uniqid(),
    'description' => $faker->text,
    'urlprefix' => $faker->url,
    'url' => $faker->url,
  ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.dbxref', function (Faker\Generator $faker) {
  return [
    'db_id' => factory('chado.db')->create()->db_id,
    'accession' => $faker->numberBetween(),
    'version' => $faker->numberBetween(),
    'description' => $faker->text,
  ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.cvterm', function (Faker\Generator $faker) {
  return [
    'cv_id' => factory('chado.cv')->create()->cv_id,
    'dbxref_id' => factory('chado.dbxref')->create()->dbxref_id,
    'name' => $faker->word,
    'definition' => $faker->text,
    'is_obsolete' => 0,
    'is_relationshiptype' => 0,
  ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.organism', function (Faker\Generator $faker) {

  $genus = $faker->word;
  $species = $faker->word;

  $abbr = substr($genus, 0) + ". " + $species;

  return [
    'abbreviation' => $abbr,
    'genus' => $genus,
    'species' => $faker->name,
    'common_name' => $faker->word,
    'type_id' => factory('chado.cvterm')->create()->cvterm_id,
  ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.feature', function (Faker\Generator $faker) {
  return [
    'name' => $faker->word,
    'uniquename' => $faker->unique()->word,
    'organism_id' => factory('chado.organism')->create()->organism_id,
    'type_id' => factory('chado.cvterm')->create()->cvterm_id,
  ];
});


Factory::define('chado.analysis', function (Faker\Generator $faker) {
  return [
    'name' => $faker->word,
    'description' => $faker->text,
    'program' => $faker->unique()->word,
    'programversion' => $faker->unique()->word,
    'sourcename' => $faker->unique()->word,
    'algorithm' => $faker->word,
    'sourcename' => $faker->word,
    'sourceversion' => $faker->word,
    'sourceuri' => $faker->word,
    // 'timeexecuted' => $faker->time()// needs to match 2018-03-23 15:08:00.000000
  ];
});

Factory::define('chado.contact', function (Faker\Generator $faker) {
  return [
    'type_id' => factory('chado.cvterm')->create()->cvterm_id,
    'name' => $faker->name,
    'description' => $faker->text,
  ];
});


Factory::define('chado.biomaterial', function (Faker\Generator $faker) {
  return [

    'taxon_id' => factory('chado.organism')->create()->organism_id,
    'biosourceprovider_id' => factory('chado.contact')->create()->contact_id,
    'dbxref_id' => factory('chado.dbxref')->create()->dbxref_id,
    'name' => $faker->unique()->word,
    'description' => $faker->text,

  ];
});


Factory::define('chado.featuremap', function (Faker\Generator $faker) {
  return [

    'name' => $faker->unique()->word,
    'description' => $faker->text,
    'unittype_id' => factory('chado.cvterm')->create()->cvterm_id,
  ];
});

Factory::define('chado.featurepos', function (Faker\Generator $faker) {
  return [
    'featuremap_id' => factory('chado.featuremap')->create()->featuremap_id,
    'feature_id' => factory('chado.feature')->create()->feature_id,
    'map_feature_id' => factory('chado.feature')->create()->feature_id,
    'mappos' => $faker->randomFloat,
  ];
});


// IMPORTANT!!!!
// IF you use this factory, call 
//
//  $prev_db = chado_set_active('chado');
//
// beforehand, and 
//
//  chado_set_active($prev_db);
//
// afterwards.


Factory::define('chado.featureloc', function (Faker\Generator $faker) {
  

 $a = $faker->randomNumber;
 $b = $faker->randomNumber;

 return [

   'feature_id' => factory('chado.feature')->create()->feature_id,
   'srcfeature_id' => factory('chado.feature')->create()->feature_id,
   'fmin' => min([$a, $b]),
   'is_fmin_partial' => 0,
   'fmax' => max([$a, $b]),
   'is_fmax_partial' => 0,
   'strand' => NULL,
   'phase' => NULL,
   'residue_info' => $faker->word,
   'locgroup' => 0,
   'rank' => 0,
  ];
  
});

Factory::define('chado.library', function (Faker\Generator $faker) {
  return [
    'organism_id' => factory('chado.organism')->create()->organism_id,
    'name' => $faker->word,
    'uniquename' => $faker->unique()->word,
    'type_id' => factory('chado.cvterm')->create()->cvterm_id,
    'is_obsolete' => 0,
  ];
});

Factory::define('chado.project', function (Faker\Generator $faker) {
  return [

    'name' => $faker->word,
    'description' => $faker->text,
  ];
});
