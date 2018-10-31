# Tripal JBrowse
This module provides integration between Drupal/Tripal sites and pre-existing GMOD Jbrowse instances. 

## Dependencies
* [Link Field](https://www.drupal.org/project/link)

## Installation
1. First install the link module.
2. Then install this module as you would any other Drupal module ([Drupal Documentation](https://www.drupal.org/documentation/install/modules-themes/modules-7))

## Functionality
This module creates a "JBrowse Instance" content type with fields (uses the Drupal Field API) for specifying the URL of the pre-existing JBrowse instance, the start location and the tracks to display by default. There is extensive documentation in the add/edit.

![Screenshot of Add/Edit Form](https://github.com/UofS-Pulse-Binfo/tripal_jbrowse/blob/7.x-2.1.x/theme/images/tripal_jbrowse.edit_form.screenshot.png)

This allows you to create a page for each JBrowse Instance you want to embed in your site. You can use the Drupal "URL path alias" settings on the add/edit form to provide a friendly URL and theme it the same way you would any other node page.

![Screenshot of JBrowse Instance Page](https://github.com/UofS-Pulse-Binfo/tripal_jbrowse/blob/7.x-2.1.x/theme/images/tripal_jbrowse.page.screenshot.png)

## Future Work
1. Embed JBrowse natively (not inside an iFrame) --depends on https://github.com/GMOD/jbrowse/issues/777
2. Create a Drupal 7 field (perhaps a Tripal3 TripalField) which embeds a JBrowse on Tripal content pages.
NOTE: This module is already Tripal 3 compatible. The next version will continue to use a node for the JBrowse instance page since it is simply a page and not biological content.
