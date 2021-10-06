
var connection = new WebSocket('ws://'+location.hostname+':81/');

var button_1_status = 0;
var button_2_status = 0;
var sens_data = 0;
var vaz_data = 0;
connection.onmessage = function(event){

  var full_data = event.data;
  console.log(full_data);
  var data = JSON.parse(full_data);
  sens_data = data.sens;
  vaz_data = data.vaz;
  document.getElementById("sensor_meter").value = sens_data;
  document.getElementById("sensor_value").innerHTML = sens_data;
    document.getElementById("vazao_meter").value = vaz_data;
  document.getElementById("vazao_value").innerHTML = vaz_data;
}

