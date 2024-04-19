![PHPUnit](https://github.com/tripal/tripal_jbrowse/workflows/PHPUnit/badge.svg)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/tripal/tripal_jbrowse)

[![DOI](https://zenodo.org/badge/44405693.svg)](https://zenodo.org/badge/latestdoi/44405693)

# Tripal JBrowse Integration

This package of modules integrates [GMOD JBrowse](https://jbrowse.org/) into your [Tripal](http://tripal.info/) site providing
 - Tripal page integration via **Tripal JBrowse Page** and
 - *Coming soon*: a user interface for JBrowse instance creation and management via **Tripal JBrowse Management**.

 This powerful combination allows you to provide seamless genome browsing to your users in an administrator-friendly manner.

## Installation Instructions

### Quickstart using Docker
If you do not yet have a Tripal 4 site and want to try out our module, you can use the [Tripal Docker](https://tripaldoc.readthedocs.io/en/latest/install/docker.html). By using the `Dockerfile` within this repository, you will simulate a Tripal 4 website with Tripal JBrowse already enabled. All you need to have installed on your local machine is [Docker or Docker Desktop](https://docs.docker.com/get-docker)!

1. Clone this repository and cd into it
```
git clone https://github.com/tripal/tripal_jbrowse.git
cd tripal_jbrowse
```
2. Build the docker image (this may take a few minutes)
```
docker build . --tag=tripal_jbrowse:latest
```
3. Run the docker container off the image you just built. This run command maps your current directory (ie. this repository) into the container so it will be possible to edit the repository locally (this is particularly useful if you want to help contribute to this module!). You can also change port 80 to another port if you need (ex: 9000:80).
```
docker run -dit --name=tripaljbrowse --publish=80:80 --volume=`pwd`:/var/www/drupal/web/modules/contrib/tripal_jbrowse tripal_jbrowse:latest
```
4. Restart your docker container's postgresql database
```
docker exec tripaljbrowse service postgresql restart
```
5. Now you can navigate to http://localhost/ to see your Tripal JBrowse docker up and running! NOTE: If you changed the port number in step 3, you will navigate to localhost:<port #> instead (ex: http://localhost:9000).

From here, if you'd like to follow a tutorial on how to embed an example JBrowse, follow the instructions here: Tutorial.md

If you'd like to contribute to the module's development, refer to the contribute section below.

### If you have an existing Tripal 4 site

Navigate to your site’s base directory (this is called “drupal” in the Tripal Docker – you'll want to use `cd ../` to get there after you first enter the docker via command line). Then run:
`composer require tripal/tripal_jbrowse`

You can now enable the module by navigating to **Administration » Manage » Extend** and scrolling down to the “Tripal JBrowse” heading. Click the checkbox next to “Tripal JBrowse” (ignore Tripal JBrowse Management for now), scroll all the way down and click “Install”.

Congratulations, you’ve now installed Tripal JBrowse!

At present, you do not need to install JBrowse2 on the same machine that your Tripal 4 site is located. Instead, a URL to an external configuration file is used by the module to embed that instance. There are current plans to accomodate access to locally hosted JBrowse2 instances, as was done in version 7.x-3.x of this module.

## Listing and Creating Instances

To list all instances, visit **Administration » Tripal » Content » JBrowse Instances**

An alternative way to get there is **Administration » Manage » Content » JBrowse Instances**

You can create a new JBrowse instance from this page (using the “+ Add jbrowse instance” button). Note that your Tripal site will need to have an organism and genome assembly already created in order to associate it with your JBrowse instance. (Both of these can be created through **Administration » Manage » Content » Tripal Content** and clicking on **+ Add Tripal Content**)

## Editing or Deleting an Instance

On the JBrowse Instances listing, you can edit or delete an existing JBrowse instance by selecting “Edit” or “Delete”, respectively, in the dropdown menu under “Operations”.

You can also perform these operations from the Instance page itself, by selecting the Edit or Delete tab located next to the View tab at the top of the page.

## To contribute to development using the tripal_jbrowse docker
Refer to the steps above in `Quickstart using Docker` to setup a docker container for development. Refer to the [Tripal Docker Documentation](https://tripaldoc.readthedocs.io/en/latest/install/docker.html) for information on how to log in and administer the site, as well as any troubleshooting.

This setup enables you to open the repository files locally in your favourite editor and any changes should show up in http://localhost. You can also setup a new docker container at any time with existing code changes to your repository (useful if you need to quickly reset any configuration changes or content added to your Tripal 4 site within the docker).

If you have trouble seeing changes you've made to the code, try running a cache rebuild on the site:
```
docker exec tripaljbrowse drush cr
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
