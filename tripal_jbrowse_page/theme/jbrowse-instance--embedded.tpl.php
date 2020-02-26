<?php
/**
 * Provides an embedded JBrowse instance.
 *
 * Variables:
 *  - $url: the url of the JBrowse instance.
 *  - $instance: an object describing the jbrowse instance.
 */
?>

<?php if ($instance->analysis_id > 0) : ?>
  <h3>Sequence Assembly:
    <?php if ($instance->analysis->url) : ?>
      <a href="<?php print $instance->analysis->url; ?>"><?php print $instance->analysis->name; ?></a>
    <?php else: ?>
      <?php print $instance->analysis->name; ?>
    <?php endif; ?>
  </h3>
<?php endif; ?>
<p><?php print $instance->description; ?></p>
<div id="JBrowseInstance">
  <iframe src="<?php print $url;?>" width="100%" height="100%">
  </iframe>
</div>
