/*
-------------------- webpage.js Architecture: --------------------

0. Fundamentals
  [X] Find all the necessary variables
  [ ] Define helper functions to execute update()

I. Initial page needs to contain the statistics and chart information
  [ ]

II. Chart
  [ ] Calculate offset

III. Define and link helper functions for the settings
  [ ] How to achieve auto-update? What structure should I use?
  [ ] Add unclick functionality

*/

/* --------------------------------------------------------------- */
/* -------------------------- Variables -------------------------- */
var usage_max = 0, usage_avg = 0, usage_latest = 0;

var config = {
  width : 300,
  height : 300,
  background_color : 'gray',
  bar_color : 'green',
  xAxis_len : 200,
  yAxis_len : 300
};
// var config = char

var auto_update = true;
var threshold = undefined;
var frequency = 1; // communication frequency and display interval


/* --------------------------------------------------------------- */
/* ------------------------- Setup Chart ------------------------- */
var svg = d3.select('svg');
// svg.selectAll('rect')
//   .attr('width', 200)
//   .attr('height', 200)
//   .attr('style', 'fill:gray');

// x-axis
var xScale = d3.scaleLinear()
  .domain([0, 1]) // value
  .range([0, config.xAxis_len]); // length
  // .attr('transform', 'translate(0, 200)');
var xAxis = d3.axisBottom(xScale);

svg.append('g')
  .attr('transform', 'translate(33, 300)') // TODO: Dynamify the configurations
  .call(xAxis);

// y-axis
var yScale = d3.scaleLinear()
  .domain([100, 0]) // value
  .range([0, config.yAxis_len]); // length
var yAxis = d3.axisRight(yScale);

svg.append('g')
  .attr('transform', 'translate(33, 33)')
  .call(yAxis);



/* -------------------------------------------------------------- */
/* ---------------------------- test ---------------------------- */
// $('#usage_max').val('23').css('color', 'red');
// while (true){
  // setTimeout(resolve, 100);
  // new Promise(resolve => setTimeout(resolve, 1000));
  // sleep(frequency);
  // document.write('1 second passed \n');

// }


/* -------------------------------------------------------------- */
/* --------------------- Callback functions --------------------- */
/* Helper function to automatically execute the update operation */


function update(){
  // 1. Communicate
  communication();
  // 2. Update the display
  update_numbers();
  update_chart();
  if (threshold && usage_lates > threshold){
    alarm_threshold();
  }

}

function change_frequency(){
  frequency = $('frequency_input').val(); // udpate frequency
  $('frequency_input').css('color', 'green'); // visual cue  // TODO: check if change color
  update_chart();
}


/* -------------------------------------------------------------- */
/* ---------------------- Helper functions ---------------------- */
function communication(){

}

function update_numbers(){

}

function update_chart(){
  // Note: update x-axis range
}

function alarm_threshold(){

}





/* -------------------------------------------------------------- */
/* --------------------------- jQuery --------------------------- */
