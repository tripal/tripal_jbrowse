![PHPUnit](https://github.com/tripal/tripal_jbrowse/workflows/PHPUnit/badge.svg)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/tripal/tripal_jbrowse)

[![DOI](https://zenodo.org/badge/44405693.svg)](https://zenodo.org/badge/latestdoi/44405693)

# Tripal JBrowse Integration

This package of modules integrates [GMOD JBrowse](https://jbrowse.org/) into your [Tripal](http://tripal.info/) site providing
 - Tripal page integration via **Tripal JBrowse Page** and
 - *Coming soon*: a user interface for JBrowse instance creation and management via **Tripal JBrowse Management**.

 This powerful combination allows you to provide seamless genome browsing to your users in an administrator-friendly manner.

## Installation Instructions
If you do not yet have a Tripal 4 site and want to try out our module, you can use the Tripal Docker. Please see the documentation to set it up: https://tripaldoc.readthedocs.io/en/latest/install/docker.html 

Assuming you have a Tripal 4 site, navigate to your site’s base directory (this is called “drupal9” in the Tripal Docker – you'll want to use `cd ../` to get there after you first enter the docker via command line). Then run: 
`composer require tripal/tripal_jbrowse` 

You can now enable the module by navigating to **Administration » Manage » Extend** and scrolling down to the “Tripal JBrowse” heading. Click the checkbox next to “Tripal JBrowse” (ignore Tripal JBrowse Management for now), scroll all the way down and click “Install”.

Congratulations, you’ve now installed Tripal JBrowse! 

At present, you do not need to install JBrowse2 on the same machine that your Tripal 4 site is located. Instead, a URL to an external instance is used by the module to embed that instance. There are current plans to implement path-based access to JBrowse2 instances, as was done in version 7.x-3.x of this module. 

## Listing and Creating Instances 

To list all instances, visit **Administration » Tripal » Content » JBrowse Instances**

An alternative way to get there is **Administration » Manage » Content » JBrowse Instances** 

You can create a new JBrowse instance from this page (using the “+ Add jbrowse instance” button). Note that your Tripal site will need to have an organism and genome assembly already created in order to associate it with your JBrowse instance. (Both of these can be created through **Administration » Manage » Content » Tripal Content** and clicking on **+ Add Tripal Content**)

## Editing or Deleting an Instance 

On the JBrowse Instances listing, you can edit or delete an existing JBrowse instance by selecting “Edit” or “Delete”, respectively, in the dropdown menu under “Operations”.  

You can also perform these operations from the Instance page itself, by selecting the Edit or Delete tab located next to the View tab at the top of the page.

## To contribute to development using the tripal_jbrowse docker
All you need to have installed locally is [Docker or Docker Desktop](https://docs.docker.com/get-docker)!

1. Build the image and start a container.
```
docker build . --tag=tripal_jbrowse:latest
```
2. The run command maps your current directory into the container so you can edit it locally. The tripal_jbrowse repository should be your current repository.
```
docker run -dit --publish=80:80 --volume=`pwd`:/var/www/drupal9/web/modules/contrib/tripal_jbrowse tripal_jbrowse:latest
```

## License & Acknowledgements

This module is open-source and licensed under GPLv3. [Read full license](LICENSE.txt).

**4.x Authors:**
- Carolyn Caron (@carolyncaron)
- Lacey-Anne Sanderson (@laceysanderson)

**7.x-3.x Authors:**

- Lacey-Anne Sanderson (@laceysanderson)
- Abdullah Almsaeed (@almasaeed2010)
- Joe West (@jwest60)
- Other contributors (https://github.com/tripal/tripal_jbrowse/graphs/contributors)

*Copyright 2018 University of Saskatchewan and University of Tennessee Knoxville.*

**Citation:**

Lacey-Anne Sanderson, Abdullah Almsaeed, Joe West, & Yichao Shen. (2019). tripal/tripal_jbrowse: Tripal JBrowse 3.0 (Version 7.x-3.0). Zenodo. http://doi.org/10.5281/zenodo.3564724.
