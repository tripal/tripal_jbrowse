<p>The JBrowse genome browser allows you to visually explore genomes and their associated large-scale datasets. JBrowse is a widely used application that is fast, intuitive, and compatible with most web browsers.</p>

<div class="jbrowse-list">

  <?php foreach ($instances as $instance) { ?>

    <div class="jbrowse-instance">
      <h3><?php print l($instance->title, $instance->url); ?></h3>
      <p><?php
      if(property_exists($instance, 'analysis')) {
        print 'Analysis: ' . $instance->analysis->name . "<br>";
      }
      print $instance->description; ?></p>
      <span class="jbrowse-launch-link"><?php print l('Launch JBrowse', $instance->url); ?></span>
    </div>

  <?php }
    if (empty($instances)) {?>

      <div class="empty-list">
        <p>There are currently no available JBrowse instances.</p>
      </div>
  <?php } ?>
</div>


<div class="jbrowse-admin-message">
  <?php
    print tripal_set_message(
      'You can create or register a JBrowse Instance at '
      .l('Administration Toolbar > Tripal > Extensions > Tripal JBrowse Management', 'admin/tripal/extension/tripal_jbrowse/management'),
      TRIPAL_INFO,
      ['return_html' => TRUE]
    );
  ?>
</div>
