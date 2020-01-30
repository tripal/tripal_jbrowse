Tripal JBrowse Nodes Migration
================================

The old method of Tripal JBrowse page integration using nodes has been **deprecated**. If you are still using this method of integration, administrators will see a warning directing you here.

The new method of page integration using the Tripal JBrowse Management module to register instances with your Tripal site. The following steps will help you register your existing JBrowse instances with Tripal JBrowse Management in order to take advantage of the new page integration.

1. Enable Tripal JBrowse Management as you would any other Drupal/Tripal module. This can be done using ``drush pm-enable tripal_jbrowse_mgmt``.
2. Download JBrowse and install a single instance in a web-accessible location. We recommend ``DRUPAL_ROOT/tools/jbrowse``.
3. Create symbolic links to each of your existing instances where the name of the link matches the organism it is for: ``genus_species__common_name``.

.. code::

  cd DRUPAL_ROOT/tools/jbrowse
  mkdir data
  cd data
  ln -s path/to/existing/instance/data/directory genus_species__common_name

4. Navigate to Administration » Tripal » Extensions » Tripal JBrowse and click on "Settings". Fill out the paths for your new single JBrowse instance.

+------------------------------------+---------------------------------------+
| Data Directory                     | DRUPAL_ROOT/tools/jbrowse/data        |
+------------------------------------+---------------------------------------+
| Data Path                          | ./tools/jbrowse/data                  |
+------------------------------------+---------------------------------------+
| Path to JBrowse's Index File       | ./tools/jbrowse                       |
+------------------------------------+---------------------------------------+
| Path to JBrowse's bin Directory    | DRUPAL_ROOT/tools/jbrowse/bin         |
+------------------------------------+---------------------------------------+

5. Navigate to Administration » Tripal » Extensions » Tripal JBrowse and click on "Register Existing Instance". Fill in the details for each of your existing instances.
6. Enable the new Tripal JBrowse Page Integration module and dynamic pages will now be available for each of your newly registered instances. This can be done using ``drush pm-enable tripal_jbrowse_page``.
7. Delete your existing Tripal JBrowse Instance nodes under Admin » Content and disable the old module using ``drush pm-disable tripal_jbrowse``.
