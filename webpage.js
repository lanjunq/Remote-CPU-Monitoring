/* --------------------------------------------------------------- */
/* -------------------------- Variables -------------------------- */
// CPU usage variables
var usage_max = undefined;
var usage_avg = undefined;
var usage_latest = undefined;
var count = undefined;

// webpage property variables
var auto_update = false;            // auto-update is on(true) or off(false)
var threshold = undefined;          // threshold for cpu usage alarm
var frequency = 1;                  // auto-update frequency (second)
var auto_update_handle = undefined; // handle to stop auto-update

// visual configuration variables
var config = {
  width : 300,                      // svg width
  height : 400,                     // svg height
  background_color : 'gray',        //
  bar_color : 'green',              //
  xAxis_len : 200,                  // length of x-axis
  yAxis_len : 300,                  // length of y-aixs
  x_offset : 50,                    // offset from left-boudnary of svg
  y_offset : 320                    // offset from top-boudnary of svg
};


/* --------------------------------------------------------------- */
/* ------------------------- Setup Chart ------------------------- */

// select svg and set width and height
var svg = d3.select('svg')
  .attr('width', `${config.width}`)
  .attr('height', `${config.height}`);

// Setup x-axis
var xScale = d3.scaleLinear()
  .domain([0, 1])                 // x-axis value
  .range([0, config.xAxis_len]);  // x-axis length
var xAxis = d3.axisBottom(xScale);
// Draw x-axis
svg.append('g')
  .attr('transform', `translate(${config.x_offset}, ${config.y_offset})`) // calculate x-axis location
  .call(xAxis);

// Setup y-axis
var yScale = d3.scaleLinear()
  .domain([100, 0])               // y-axis value
  .range([0, config.yAxis_len]);  // y-axis length
var yAxis = d3.axisLeft(yScale);
// Draw y-axis
svg.append('g')
  .attr('transform', `translate(${config.x_offset}, ${config.y_offset-config.yAxis_len})`) // calculate y-axis location
  .call(yAxis);

// Initialize bar display
svg.append('g').append('rect')
  .attr('transform', `translate(${config.x_offset}, ${(1-usage_latest/100) * config.yAxis_len})`) // Initialize the bar display
  .attr('width', `${config.xAxis_len}`)
  .attr('height', '0')
  .style('fill', '#95f095'); // bar color


/* -------------------------------------------------------------- */
/* --------------------- Callback functions --------------------- */

/* Helper function to automatically execute the update operation */
function enable_auto_update () {
  auto_update = true;
  auto_update_handle = setInterval(update, frequency * 1000); // enable auto-update
  $('#auto_update_status').removeClass('off');  // visual changes to remind user
  $('#auto_update_status').addClass('on');
  $('#auto_update_status').val('On');
}

/* Helper function to stop auto-update (not yet used) */
function disable_auto_update(){
  clearInterval(auto_update_handle);
  auto_update = false;
  $('#auto_update_status').addClass('off');   //
  $('#auto_update_status').removeClass('on'); //
  $('#auto_update_status').val('Off');        //
}

function set_threshold(){
  // Disable alarm
  if (threshold){                             // if the threshold is truthy value (not undefined)
    threshold = undefined;
    $('#threshold_input').removeClass('on');  // remove back-ground color green
  }
  // Enable alarm
  else {
    threshold = $('#threshold_input').val();  // read threshold value from box
    $('#threshold_input').addClass('on');     // change background-color to green
  }
}

/* Callback function to change the page 'refresh requency' */
function change_frequency(){
  frequency = $('#frequency_input').val();    // frequency as in text
  frequency = parseInt(frequency, 10);        // frequency as in number
  $('#frequency_input').addClass('on');       // change background-color to green
  refresh_update_frequency();                 // call helper function
}


/* -------------------------------------------------------------- */
/* ---------------------- Helper functions ---------------------- */

/* Helper function for auto-update. update() is called each time the
  webpage needs to be 'refreshed' */
function update(){
  communication();
  update_numbers();
  update_chart();
  threshold_alarm();
}

/* Helper function to update the chart display section on the webpage */
function update_chart(){
  $('rect')
    .attr('transform', `translate(${config.x_offset+1}, ${config.y_offset-usage_latest/100 * config.yAxis_len})`)
    .attr('height', `${usage_latest/100 * config.yAxis_len}`);
}

/* Helper function to send HTTP request to the server
  and update the CPU statistics */
function communication(){
  $.getJSON("/data", function (data, status) {    // send HTTP request
    count = data.count;                           // update the CPU statistics
    usage_max = data.usage_max;
    usage_avg = data.usage_avg;
    usage_latest = data.usage_latest;
    update_numbers(); // QUESTION: Does this cause update frequency problem?
    update_chart();
  })
    .fail(function() {
			disable_auto_update();
      console.log('getJSON fails');
			$.getJSON("");
    })
}

/* Helper function to udpate the CPU statistics section on the webpage */
function update_numbers(){
  $('#usage_max').val(usage_max);
  $('#usage_avg').val(Math.round(usage_avg * 100) / 100);
  $('#usage_latest').val(usage_latest);
}

/* Helper function to alarm CPU usage pass threshold */
function threshold_alarm(){
  if (threshold && usage_latest > threshold){
    alert("Latest CPU usage exceeds the threshold: " + threshold);
    // QUESTION: Do I cancel alarm after one-time alarm?
  }
}

/* Helper function to update auto-update frequency when user inputs a new frequency */
function refresh_update_frequency(){
  if (auto_update){
    clearInterval(auto_update_handle);
    auto_update_handle = setInterval(update, frequency * 1000);
  }
}


/* -------------------------------------------------------------- */
/* --------------------------- jQuery --------------------------- */
// Associate buttons to corresponding callback functions
$('#auto_update_button').click(enable_auto_update);
$('#threshold_button').click(set_threshold);
$('#frequency_button').click(change_frequency);

update();
