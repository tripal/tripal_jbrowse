# Steps to embed an example JBrowse 
This tutorial will walk you through how to embed an example JBrowse instance using this module. All steps shown here take place on the Tripal 4 site directly. As new functionality is added to the module, this tutorial should also be updated. If you need clarification of any steps or have any suggestions to improve this tutorial, please reach out to us through the `Issues` tab and create an issue!

## Setup
Please follow the instructions from the README on how to either:
1. Setup a Docker environment with Tripal 4 + Tripal JBrowse
2. How to install Tripal JBrowse on a pre-existing Tripal 4 site

Make sure you are logged in as an administrator. If you are using the tripaldocker environment, log in as an administrator using the [credentials found here](https://tripaldoc.readthedocs.io/en/latest/install/docker.html#development-site-information)

## Our JBrowse example
We've sourced an example JBrowse configuration file from the [repository for JBrowse2 here](https://github.com/GMOD/jbrowse-components/blob/main/test_data/config.json) and included it in this repository for convenience as `example_config.json`.

## Create an Organism
Your first task is to make sure you have an organism page for the organism of your JBrowse instance. From the Administration toolbar at the top of the page, click on **Tripal**, then **Content** in the toolbar that drops down. Then click on the button `+Add Tripal Content` and select **Organism** under `General`.

Our example JBrowse is of a human genome assembly, so in **Genus** put "Homo" and in **Species** put "sapiens". You can fill out the other fields if you'd like but they aren't required, then hit **Save** at the bottom.

![Screenshot_addTripalContent](/Tutorial/Screenshot_addTripalContent.png)

You will be directed to an organism page for Homo sapiens - yay!

## Create a Genome Assembly
Just as with creating an organism, navigate to the `Add Tripal Content` page. Under the `Genomic` heading, click on **Genome Assembly**. Though not required, I recommend giving a name to your genome assembly to make it easily to recognize. In this case: `hg19`. Then fill out the required fields with the following (or really, whatever you'd like!):
- **Program, Pipeline, Workflow or Method Name**: `My pipeline`
- **Program Version**: `1.0`

And hit **Save**. You are now at your newly created Genome Assembly page for the hg19 assembly!

## Create a JBrowse Instance
Now return to **Tripal** then **Content**. Above the blue buttons, you'll see multiple tabs and you should currently be on the `Tripal Content` tab. Click on the `JBrowse Instances` tab and then on the blue button `+Add jbrowse instance`.

Give your JBrowse instance a title and optional description. 
- **JBrowse2 Configuration URL**: http://localhost/modules/contrib/tripal_jbrowse/Tutorial/example_config.json
NOTE: If you set up your docker container on a different port than 80, then your URL will include that port number after `localhost:`. For example, for port 9000, it will be: http://localhost:9000/modules/contrib/tripal_jbrowse/Tutorial/example_config.json
Here, you are essentially hosting the example config file on your docker directly through localhost! If you wanted to try this with a valid configuration file hosted elsewhere, you can add that URL instead. You will want to ensure the host server cross origin resource sharing (CORS) enabled in order for this to work. Refer to this page in the [JBrowse documentation here](https://jbrowse.org/docs/cors.html)

- **Default Start Location**: This can be left empty, or you can set it using this format: `<chr>:<start pos>..<end pos>` For example, `1:1..100,000,000` will start the JBrowse session at chromosome 1 between the 1st and 100,000,000 nucelotide.

- **Organism**: Begin typing "Homo sapiens" and choose it from the dropdown (it amy take a second or more to appear). It's important to select it rather than just typing it out, otherwise you'll encounter an error when submitting the form.

- **Genome Assembly**: Begin typing "hg19" and select it from the dropdown.

![Screenshot_addJBrowseInstance](/Tutorial/Screenshot_addJBrowseInstance.png)

Now click **Save** 

Hopefully, you will be rewarded with a JBrowse2 instance like the one below:
![Screenshot_embeddedJBrowse](/Tutorial/Screenshot_embeddedJBrowse.png)

