/* --------------------------------------------------------------- */
/* -------------------------- Variables -------------------------- */
var usage_max = undefined, usage_avg = undefined, usage_latest = undefined;

var config = {
  width : 300,
  height : 400,
  background_color : 'gray',
  bar_color : 'green',
  xAxis_len : 200,
  yAxis_len : 300,
  x_offset : 50,
  y_offset : 320
};


var auto_update = false;
var threshold = undefined;
var frequency = 1; // communication frequency and display interval
var auto_update_handle;

/* --------------------------------------------------------------- */
/* ------------------------- Setup Chart ------------------------- */
var svg = d3.select('svg')
// svg.selectAll('rect')
  .attr('width', `${config.width}`)
  .attr('height', `${config.height}`);
//   .attr('style', 'fill:gray');

// x-axis
var xScale = d3.scaleLinear()
  .domain([0, 1]) // value
  .range([0, config.xAxis_len]);
var xAxis = d3.axisBottom(xScale);

svg.append('g')
  .attr('transform', `translate(${config.x_offset}, ${config.y_offset})`)
  .call(xAxis);

// y-axis
var yScale = d3.scaleLinear()
  .domain([100, 0]) // value
  .range([0, config.yAxis_len]);
var yAxis = d3.axisLeft(yScale);

svg.append('g')
  .attr('transform', `translate(${config.x_offset}, ${config.y_offset-config.yAxis_len})`)
  .call(yAxis);

// empty bar
svg.append('g').append('rect')
  .attr('transform', `translate(${config.x_offset}, ${(1-usage_latest/100) * config.yAxis_len})`)
  .attr('width', `${config.xAxis_len}`)
  // .attr('width', '140')
  .attr('height', '0');
  // .attr('height', '40');



/* -------------------------------------------------------------- */
/* ---------------------------- test ---------------------------- */



/* -------------------------------------------------------------- */
/* --------------------- Callback functions --------------------- */
/* Helper function to automatically execute the update operation */

function enable_auto_update () {
  console.log('Start Auto-update');
  auto_update = true;
  auto_update_handle = setInterval(update, frequency * 1000);
  $('#auto_update_status').removeClass('off');
  $('#auto_update_status').addClass('on');
  $('#auto_update_status').val('On');
}

function refresh_update_frequency(){
  if (auto_update){
    // QUESTION: Do I need to clear the previous setInterval?
    clearInterval(auto_update_handle);
    auto_update_handle = setInterval(update, frequency * 1000);
  }
}

function disable_auto_update(){
  clearInterval(auto_update_handle);
  auto_update = false;
  $('#auto_update_status').addClass('off');
  $('#auto_update_status').removeClass('on');
  $('#auto_update_status').val('Off');
}

function set_threshold(){
  if (threshold){ // Disable alarm
    threshold = undefined;
    $('#threshold_input').removeClass('on');
  } else { // Enable alarm
    threshold = $('#threshold_input').val();
    $('#threshold_input').addClass('on');
  }
}

function change_frequency(){
  frequency = $('#frequency_input').val(); // frequency as in text
  frequency = parseInt(frequency, 10); // frequency as in number
  $('#frequency_input').addClass('on');
  refresh_update_frequency();
  // update();
}


/* -------------------------------------------------------------- */
/* ---------------------- Helper functions ---------------------- */
function update(){
  communication();
  update_numbers();
  update_chart();
  threshold_alarm();
}

function update_numbers(){
  $('#usage_max').val(usage_max);
  $('#usage_avg').val(usage_avg);
  $('#usage_latest').val(usage_latest);
}

function update_chart(){
  $('rect')
    .attr('transform', `translate(${config.x_offset}, ${config.y_offset-usage_latest/100 * config.yAxis_len})`)
    .attr('height', `${usage_latest/100 * config.yAxis_len}`);
}

function communication(){
  $.getJSON("/data", function (data, status) {
    count = data.count;
    usage_max = data.usage_max;
    usage_avg = data.usage_avg;
    usage_latest = data.usage_latest;
  })
}

function threshold_alarm(){
  if (threshold && usage_latest > threshold){
    alert("Latest CPU usage exceeds the threshold: " + threshold);
    // QUESTION: Do I cancel alarm after one-time alarm?
  }
}


/* -------------------------------------------------------------- */
/* --------------------------- jQuery --------------------------- */
update();
$('#auto_update_button').click(enable_auto_update);
$('#threshold_button').click(set_threshold);
$('#frequency_button').click(change_frequency);
