Drupal.behaviors.embedJBrowse = {
  attach: function (context, settings) {
    // Use context to filter the DOM to only the elements of interest,
    // and use once() to guarantee that our callback function processes
    // any given element one time at most, regardless of how many times
    // the behaviour itself is called (it is not sufficient in general
    // to assume an element will only ever appear in a single context).
    once('embedJBrowse', '#jbrowse_linear_genome_view', context).forEach(
      function (element) {
        fetch(drupalSettings.jbrowseUrl)
        .then(response => {
          if (!response.ok) {
            throw new Error('Network response was not ok');
          }
          return response.json();
        })
        .then(data => {
          // A site on a different server may be using relative URLs,
          // so prefix with JBrowse base URL if necessary.
          const jbrowseBase = new URL('.', drupalSettings.jbrowseUrl).href;
          absolutizeUris(data, jbrowseBase);

          const { createViewState, JBrowseLinearGenomeView } = JBrowseReactLinearGenomeView;
          const { createElement } = React;
          const { render } = ReactDOM;

          // Remove track types that JBrowseReactLinearGenomeView
          // does not support.
          const supportedTrackTypes = [
            'FeatureTrack',
            'VariantTrack',
            'AlignmentsTrack',
          ];
          const params = {
            assembly: data.assemblies[0],
            tracks: data.tracks.filter(track =>
              supportedTrackTypes.includes(track.type)
            ),
          };

          // If a starting location was not set, the default behavior
          // is to prompt the user to select a chromosome.
          if (drupalSettings.location) {
            params.location = drupalSettings.location;
          }

          const state = createViewState(params);
          render(
              createElement(JBrowseLinearGenomeView, { viewState: state }),
              document.getElementById('jbrowse_linear_genome_view'),
          );
        })
        .catch(error => {
          console.error('Error fetching JBrowse configuration:', error);
        });
      }
    );
  }
};

/**
 * Recursively prefixes relative urls with a base url.
 */
function absolutizeUris(obj, baseUrl) {
  if (Array.isArray(obj)) {
    obj.forEach(item => absolutizeUris(item, baseUrl));
    return;
  }

  if (obj && typeof obj === 'object') {
    Object.entries(obj).forEach(([key, value]) => {

      if (
        key === 'uri' &&
        typeof value === 'string' &&
        !value.match(/^(https?:)?\/\//)
      ) {
        obj[key] = new URL(value, baseUrl).href;
      }

      absolutizeUris(value, baseUrl);
    });
  }
}
