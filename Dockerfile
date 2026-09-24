ARG drupalversion='11.4.x'
ARG phpversion='8.5'
ARG postgresqlversion='18'
FROM tripalproject/tripaldocker:drupal${drupalversion}-php${phpversion}-pgsql${postgresqlversion}-noChado

COPY . /var/www/drupal/web/modules/contrib/tripal_jbrowse
WORKDIR /var/www/drupal/web/modules/contrib/tripal_jbrowse

ARG chadoschema='testchado'
RUN service postgresql restart \
  && drush trp-install-chado --schema-name=${chadoschema} \
  && drush trp-prep-chado --schema-name=${chadoschema} \
  && drush tripal:trp-import-types --collection_id=general_chado \
  && drush tripal:trp-import-types --collection_id=genomic_chado \
  && drush en tripal_jbrowse --yes
