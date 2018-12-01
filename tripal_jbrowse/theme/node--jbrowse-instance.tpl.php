<?php

/**
 * @file
 * JBrowse Instance page
 *
 * Available variables:
 *  - url_prefix: the URL without parameters for the JBrowse to be embedded.
 *  - location: the location to display by default
 *  - tracks: a comma-separated list of tracks to display by default
 *  - url: the full URL for the JBrowse you would like to embed
 */
?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>>
      <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
    </h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <div class="content clearfix"<?php print $content_attributes; ?>>
    <div class="jbrowse" id="GenomeBrowser">
      <div id="LoadingScreen" style="padding: 50px;">
        <h1>Loading...</h1>
      </div>
    </div>
    <div style="display: none">JBrowseDefaultMainPage</div>
  </div>

</div>
