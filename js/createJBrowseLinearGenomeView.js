Drupal.behaviors.embedJBrowse = {
  attach: function (context, settings) {
    // Use context to filter the DOM to only the elements of interest,
    // and use once() to guarantee that our callback function processes
    // any given element one time at most, regardless of how many times
    // the behaviour itself is called (it is not sufficient in general
    // to assume an element will only ever appear in a single context).
    once('embedJBrowse', '#jbrowse_linear_genome_view', context).forEach(
      function (element) {
        fetch(drupalSettings.jbrowseUrl, {
          method: 'GET',
          headers: {
            'Content-Type': 'application/json'
          }
        })
        .then(response => {
          if (!response.ok) {
            throw new Error('Network response was not ok');
          }
          return response.json();
        })
        .then(data => {
          const { createViewState, JBrowseLinearGenomeView } = JBrowseReactLinearGenomeView;
          const { createElement } = React;
          const { render } = ReactDOM;

          const params = {
            assembly: data['assemblies'][0],
            tracks: data['tracks'],
          };

          // If a starting location was not set, the default behavior
          // is to prompt the user to select a chromosome.
          if (drupalSettings.location) {
            params.location = drupalSettings.location;
          }

          const state = new createViewState(params);
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
