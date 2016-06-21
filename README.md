# Tripal JBrowse
This module provides integration between Drupal/Tripal sites and pre-existing GMOD Jbrowse instances. 

## Dependencies
* [Link Field](https://www.drupal.org/project/link)

## Installation
1. First install the link module.
2. Then install this module as you would any other Drupal module ([Drupal Documentation](https://www.drupal.org/documentation/install/modules-themes/modules-7))

##Functionality
This module creates a "JBrowse Instance" content type with fields (uses the Drupal Field API) for specifying the URL of the pre-existing JBrowse instance, the start location and the tracks to display by default. There is extensive documentation in the add/edit.
![Screenshot of Add/Edit Form](https://github.com/UofS-Pulse-Binfo/tripal_jbrowse/blob/7.x-2.1.x/theme/images/tripal_jbrowse.edit_form.screenshot.png)

This allows you to create a page for each JBrowse Instance you want to embed in your site. You can use the Drupal "URL path alias" settings on the add/edit form to provide a friendly URL and theme it the same way you would any other node page.
![Screenshot of JBrowse Instance Page](https://github.com/UofS-Pulse-Binfo/tripal_jbrowse/blob/7.x-2.1.x/theme/images/tripal_jbrowse.page.screenshot.png)

