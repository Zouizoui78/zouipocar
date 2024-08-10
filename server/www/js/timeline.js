let house = [48.7655396,2.0369118];
let line;

// Map setup
mapOptions = {
    attributionControl: false,
    center: house,
    zoom: 16,
    tap: false
}

// Tiles options
tilesOptions = {
    maxZoom: 20
}

if (utils.mobileCheck()) {
    mapOptions.zoomSnap = 0;
    tilesOptions.tileSize = 512;
    tilesOptions.zoomOffset = -1;
}

map = new L.map('map', mapOptions);
layer = new L.TileLayer('https://{s}.tile.openstreetmap.fr/osmfr/{z}/{x}/{y}.png', tilesOptions);
map.addLayer(layer);

const elem = document.querySelector('input[name="date"]');
const datepicker = new Datepicker(elem);

elem.addEventListener("changeDate", e => {
    start = e.detail.date.getTime() / 1000;
    stop = start + 86399;
    console.log(start, stop);
    ajax.get('/api/range?start=' + start + '&stop=' + stop, null, update);
});

function update(body) {
    query = JSON.parse(body);
    if (!query || query.length == 0)
        return;

    console.log("query length = " + query.length);
    polypoints = []
    distance = 0;
    time = 0;
    console.log("first timestamp = " + query[0].timestamp);
    console.log("last timestamp = " + query[query.length - 1].timestamp);
    for (k in query) {
        if (k > 0) {
            distance += distanceInKmBetweenEarthCoordinates(query[k-1].latitude, query[k-1].longitude, query[k].latitude, query[k].longitude);
            time_diff = query[k].timestamp - query[k-1].timestamp;
            if (time_diff < 600)
                time += time_diff;
            else
                ++time;
        }
        q = query[k];
        polypoints.push([q.latitude, q.longitude]);
    }

    speed = distance / time * 3600;
    time = new Date(time * 1000);

    document.getElementById("distance").innerHTML = distance.toFixed(2);
    document.getElementById("duration").innerHTML = time.toISOString().split("T")[1].split(".")[0];
    document.getElementById("speed").innerHTML = speed.toFixed(0);

    if (line != undefined)
        line.remove(map);
    line = L.polyline(polypoints).addTo(map);
    map.fitBounds(line.getBounds());
}

function degreesToRadians(degrees) {
  return degrees * Math.PI / 180;
}

function distanceInKmBetweenEarthCoordinates(lat1, lon1, lat2, lon2) {
  var earthRadiusKm = 6371;

  var dLat = degreesToRadians(lat2-lat1);
  var dLon = degreesToRadians(lon2-lon1);

  lat1 = degreesToRadians(lat1);
  lat2 = degreesToRadians(lat2);

  var a = Math.sin(dLat/2) * Math.sin(dLat/2) +
          Math.sin(dLon/2) * Math.sin(dLon/2) * Math.cos(lat1) * Math.cos(lat2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
  return earthRadiusKm * c;
}