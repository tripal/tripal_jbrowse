Managing Instances
==================

Listing and Creating Instances
------------------------------

To list all instances, visit Administration » Tripal » Extensions » Tripal JBrowse Management.

.. image:: ../../assets/instances.png


Creating Instances
------------------

As highlighted on the screenshot above, you can create new instances by clicking the "Create New Instance" link. This will create a new data directory based on the organism choosen. All instances created will use the same JBrowse installation, which makes it easier to update JBrowse.

Registering Existing Instances
--------------------------------

You can also register existing JBrowse instances with this module. To do so
1. Create a symbolic link in your configured data directory where the name of the link matches the organism it is for: ``genus_species__common_name``.
2. Click on "Register Existing Instance" as highlighted on the screenshot above.
3. Fill out the form to describe the existing JBrowse instance.

.. note::

  If you would like to move an existing instance to the JBrowse managed by this module permanently then copy the data directory rather then creating a symbolic link.

Deleting Instances
------------------

To delete an instance, you must first visit the instances list page and clicking the delete instance link.

.. warning::

  The actual files for the instance are not deleted. You will need to delete all files manually.

.. note::

  If you accidentally delete an instance, you can use the "Register Existing Instance" process to re-register it with the system. All previously created tracks will no longer be able to be managed but you will be able to add new tracks, etc.
