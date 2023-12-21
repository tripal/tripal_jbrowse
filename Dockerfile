ARG drupalversion='10.1.x-dev'
FROM tripalproject/tripaldocker:drupal${drupalversion}-php8.1-pgsql13-noChado

MAINTAINER Carolyn Caron <carolyn.caron@usask.ca>

COPY . /var/www/drupal/web/modules/contrib/tripal_jbrowse
WORKDIR /var/www/drupal/web/modules/contrib/tripal_jbrowse

ARG chadoschema='testchado'
RUN service postgresql restart \
  && drush trp-install-chado --schema-name=${chadoschema} \
  && drush trp-prep-chado --schema-name=${chadoschema} \
  && drush tripal:trp-import-types --username=drupaladmin --collection_id=general_chado \
  && drush tripal:trp-import-types --username=drupaladmin --collection_id=genomic_chado \
  && drush en tripal_jbrowse --yes
