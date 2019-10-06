<p>The JBrowse genome browser allows you to visually explore genomes and their associated large-scale datasets. JBrowse is a widely used application that is fast, intuitive, and compatible with most web browsers.</p>

<div class="jbrowse-list">

  <?php foreach ($instances as $instance) { ?>

    <div class="jbrowse-instance">
      <h3><?php print l($instance->title, $instance->url); ?></h3>
      <p><?php print $instance->description; ?></p>
      <span class="jbrowse-launch-link"><?php print l('Launch JBrowse', $instance->url); ?></span>
    </div>

  <?php } ?>
</div>
