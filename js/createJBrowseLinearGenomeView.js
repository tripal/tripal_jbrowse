// This is derived from JBrowse2 documentation for embedding a JBrowse:
// https://jbrowse.org/jb2/docs/tutorials/embed_linear_genome_view/06_creating_the_view/
// @TODO: We want to make fields for the assembly and tracks files
//        and pull those variables in
Drupal.behaviors.embedJBrowse = {
  attach: function (context, settings) {
    // Use context to filter the DOM to only the elements of interest,
    // and use once() to guarantee that our callback function processes
    // any given element one time at most, regardless of how many times
    // the behaviour itself is called (it is not sufficient in general
    // to assume an element will only ever appear in a single context).
    once('embedJBrowse', '#jbrowse_linear_genome_view', context).forEach(
      function (element) {
        console.log(drupalSettings.jbrowseUrl);
        console.log((new Date()).toLocaleString());
        Drupal.ajax({
            headers: { "Content-Type": "application/json" },
            url: drupalSettings.jbrowseUrl,
            method: "GET",
            success: function (data, status, xhr) {
                console.log("Inside success!");
                console.log(data);
                const { createViewState, JBrowseLinearGenomeView } =
                    JBrowseReactLinearGenomeView
                const { createElement } = React
                const { render } = ReactDOM
                const state = new createViewState({
                    assembly: data['assemblies'][0],
                    tracks: data['tracks'],
                    location: '1:100,987,269..100,987,368',
                })
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

