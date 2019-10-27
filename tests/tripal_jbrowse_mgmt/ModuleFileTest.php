<?php
namespace Tests\tripal_jbrowse_mgmt;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class ModuleFileTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Tests hook_menu(). Specifically, are all the required keys set.
   */
  public function testHookMenu() {

    $menu_items = tripal_jbrowse_mgmt_menu();
    $this->assertIsArray($menu_items);

    foreach($menu_items as $path => $item) {
      $this->assertArrayHasKey('title', $item,
        "$path menu item is missing a title.");
      $this->assertArrayHasKey('page callback', $item,
        "$path menu item is missing a page callback.");
      $this->assertArrayHasKey('access arguments', $item,
        "$path menu item is missing access arguments.");
      $this->assertIsArray($item['access arguments'],
        "$path menu item access arguments must be an array.");
    }
  }

  /**
   * Tests hook_permission(). Specifically, checks format.
   */
  public function testHookPerm() {
    $permissions = tripal_jbrowse_mgmt_permission();
    $this->assertIsArray($permissions);

    foreach ($permissions as $key => $perm) {
      $this->assertArrayHasKey('title', $perm,
        "$key permission is missing a title.");
    }
  }
}
