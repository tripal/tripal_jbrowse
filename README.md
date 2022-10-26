![PHPUnit](https://github.com/tripal/tripal_jbrowse/workflows/PHPUnit/badge.svg)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/tripal/tripal_jbrowse)

[![DOI](https://zenodo.org/badge/44405693.svg)](https://zenodo.org/badge/latestdoi/44405693)

# Tripal JBrowse Integration

This package of modules integrates [GMOD JBrowse](https://jbrowse.org/) into your [Tripal](http://tripal.info/) site providing
 - Tripal page integration via **Tripal JBrowse Page** and
 - a user interface for JBrowse instance creation and management via **Tripal JBrowse Management**.

 This powerful combination allows you to provide seamless genome browsing to your users in an administrator-friendly manner.

## Using the tripal_jbrowse docker
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
