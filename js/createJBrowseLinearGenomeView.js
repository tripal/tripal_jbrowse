// This is derived from JBrowse2 documentation for embedding a JBrowse:
// https://jbrowse.org/jb2/docs/tutorials/embed_linear_genome_view/06_creating_the_view/
// @TODO: We want to make fields for the assembly and tracks files
//        and pull those variables in
import assembly from '/assembly.js'
import tracks from '/tracks.js'
const { createViewState, JBrowseLinearGenomeView } =
    JBrowseReactLinearGenomeView
const { createElement } = React
const { render } = ReactDOM
const state = new createViewState({
    assembly,
    tracks,
    location: '1:100,987,269..100,987,368',
})
render(
    createElement(JBrowseLinearGenomeView, { viewState: state }),
    document.getElementById('jbrowse_linear_genome_view'),
)