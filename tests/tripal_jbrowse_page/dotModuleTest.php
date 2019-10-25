<?php
namespace Tests\tripal_jbrowse_page;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class dotModuleTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Tests tripal_jbrowse_page_menu().
   */
  public function testHookMenu() {

    $menu_items = tripal_jbrowse_page_menu();
    foreach ($menu_items as $path => $item) {

      // First ensure there are all the menu item keys required.
      $this->assertArrayHasKey('title', $item);
      $this->assertArrayHasKey('description', $item);
      $this->assertArrayHasKey('page callback', $item);
      $this->assertArrayHasKey('type', $item);
      $this->assertArrayHasKey('access arguments', $item);

      // Check that all jbrowse instance paths are the correct type.
      $path_parts = explode('/', $path);
      if (($path_parts[0] == 'jbrowse') && (sizeof($path_parts) > 1)) {
        $this->assertEquals(MENU_SUGGESTED_ITEM, $item['type']);
      }
    }
  }
}
