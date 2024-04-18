// This is derived from JBrowse2 documentation for embedding a JBrowse:
// https://jbrowse.org/jb2/docs/tutorials/embed_linear_genome_view/06_creating_the_view/

Drupal.behaviors.embedJBrowse = {
  attach: function (context, settings) {
    // Use context to filter the DOM to only the elements of interest,
    // and use once() to guarantee that our callback function processes
    // any given element one time at most, regardless of how many times
    // the behaviour itself is called (it is not sufficient in general
    // to assume an element will only ever appear in a single context).
    once('embedJBrowse', '#jbrowse_linear_genome_view', context).forEach(
      function (element) {
        Drupal.ajax({
            headers: { "Content-Type": "application/json" },
            url: drupalSettings.jbrowseUrl,
            method: "GET",
            success: function (data, status, xhr) {
                const { createViewState, JBrowseLinearGenomeView } =
                    JBrowseReactLinearGenomeView
                const { createElement } = React
                const { render } = ReactDOM
                const params = { 
                  assembly: data['assemblies'][0],
                  tracks: data['tracks'],
                }
                // If a starting location was not set, then the default behavior
                // is to prompt the user to select a chromosome
                if (drupalSettings.location) {
                  params.location = drupalSettings.location
                }
                const state = new createViewState(params)
                render(
                    createElement(JBrowseLinearGenomeView, { viewState: state }),
                    document.getElementById('jbrowse_linear_genome_view'),
                )
             }
        }).execute();
      }
    );
  }
};

