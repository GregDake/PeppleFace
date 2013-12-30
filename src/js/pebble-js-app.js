function fetchWeather(latitude, longitude) {
  	var response;
  	var req = new XMLHttpRequest();
	var url = "http://api.openweathermap.org/data/2.1/find/city?lat=" + latitude + "&lon=" + longitude + "&cnt=1";
	console.log("Url is " + url);
  	req.open('GET', url, true);
  	req.onload = function(e) {
    if (req.readyState == 4) {
    	if(req.status == 200) {
        	console.log(req.responseText);
        	response = JSON.parse(req.responseText);
        	var temperature, city;
        	if (response && response.list && response.list.length > 0) {
          		var weatherResult = response.list[0];
          		temperature = Math.round(weatherResult.main.temp - 273.15); //Subtract for kelvin
				temperature = ((9/5)*temperature)+32; //Convert to farenheight     F=(9/5)C+32
				temperature = Math.round(temperature); //No decimals
          		city = weatherResult.name;
				console.log("Temperature: " + temperature);
				console.log("City: " + city);
          		Pebble.sendAppMessage({
			   		"0":temperature + "\u00B0F",
			  		"1":city,
		  		});
        	}
      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
}

function locationSuccess(pos) {
  	var coordinates = pos.coords;
  	fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  	console.warn('location error (' + err.code + '): ' + err.message);
  	Pebble.sendAppMessage({
    	"0":"N/A",
    	"1":"Loc Unavailable",
  	});
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

Pebble.addEventListener("ready",
                        function(e) {
                        	console.log("connect!" + e.ready);
                          	locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                          	console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          	window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          	console.log(e.type);
                          	console.log(e.payload.temperature);
                          	console.log("message!");
                        });

Pebble.addEventListener("webviewclosed",
                        function(e) {
                        	console.log("webview closed");
							console.log(e.type);
                            console.log(e.response);
                        });