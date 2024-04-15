ARG drupalversion='10.2.x-dev'
ARG phpversion='8.3'
ARG postgresqlversion='16'
FROM tripalproject/tripaldocker:drupal${drupalversion}-php${phpversion}-pgsql${postgresqlversion}-noChado

COPY . /var/www/drupal/web/modules/contrib/tripal_jbrowse
WORKDIR /var/www/drupal/web/modules/contrib/tripal_jbrowse

ARG chadoschema='testchado'
RUN service postgresql restart \
  && drush trp-install-chado --schema-name=${chadoschema} \
  && drush trp-prep-chado --schema-name=${chadoschema} \
  && drush tripal:trp-import-types --username=drupaladmin --collection_id=general_chado \
  && drush tripal:trp-import-types --username=drupaladmin --collection_id=genomic_chado \
  && drush en tripal_jbrowse --yes
