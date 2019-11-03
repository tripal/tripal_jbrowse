# Tripal JBrowse Integration

This package of modules integrates [GMOD JBrowse](https://jbrowse.org/) into your [Tripal](http://tripal.info/) site providing
 - Tripal page integration via **Tripal JBrowse Page** and
 - a user interface for JBrowse instance creation and management via **Tripal JBrowse Management**.

 This powerful combination allows you to provide seamless genome browsing to your users in an administrator-friendly manner.

## Quick Start
1. Download and install JBrowse in a web accessible location (e.g. `DRUPAL_ROOT/tools/jbrowse`).
2. Download and unpack this package in your Drupal modules directory (i.e `sites/all/modules`).
3. Enable "Tripal JBrowse Management" submodule through `http://[your site]/admin/modules`.
4. Create a JBrowse instance for your species of interest using the **Admin > Tripal > Tripal JBrowse Management** user interface.
5. Enable "Tripal-JBrowse Page Integration" submodule through `http://[your site]/admin/modules`.
6. Embedded versions will be created for all your instances created in step 4.

## Documentation

Please visit our [online documentation](https://tripal_jbrowse.readthedocs.io/) to learn more about installation, usage and features.

## License & Acknowledgements

This module is open-source and licensed under GPLv3. [Read full license](LICENSE.txt).

**Authors:**

- Lacey-Anne Sanderson (@laceysanderson)
- Abdullah Almsaeed (@almasaeed2010)
- Joe West (@jwest60)
- Other contributors (https://github.com/tripal/tripal_jbrowse/graphs/contributors)

*Copyright 2018 University of Saskatchewan and University of Tennessee Knoxville.*
