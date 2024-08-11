const map = new ZouipocarMap();
let line;

const elem = document.querySelector('input[name="date"]');
const datepicker = new Datepicker(elem);

elem.addEventListener("changeDate", e => {
    const start = e.detail.date.getTime() / 1000;
    const stop = start + 86399;
    console.log(start, stop);
    ajax.get('/api/range?start=' + start + '&stop=' + stop, null, update);
});

function update(body) {
    const query = JSON.parse(body);
    if (!query || query.length == 0)
        return;

    console.log("query length = " + query.length);
    const polypoints = []
    let distance = 0;
    let time = 0;
    console.log("first timestamp = " + query[0].timestamp);
    console.log("last timestamp = " + query[query.length - 1].timestamp);
    for (k in query) {
        if (k > 0) {
            distance += distanceInKmBetweenEarthCoordinates(query[k-1].latitude, query[k-1].longitude, query[k].latitude, query[k].longitude);
            const time_diff = query[k].timestamp - query[k-1].timestamp;
            if (time_diff < 600)
                time += time_diff;
            else
                ++time;
        }
        const q = query[k];
        polypoints.push([q.latitude, q.longitude]);
    }

    const speed = distance / time * 3600;
    time = new Date(time * 1000);

    document.getElementById("distance").innerHTML = distance.toFixed(2);
    document.getElementById("duration").innerHTML = time.toISOString().split("T")[1].split(".")[0];
    document.getElementById("speed").innerHTML = speed.toFixed(0);

    if (line)
        line.remove(map);
    line = L.polyline(polypoints);
    map.map.addLayer(line);
    map.map.fitBounds(line.getBounds());
}

function degreesToRadians(degrees) {
  return degrees * Math.PI / 180;
}

function distanceInKmBetweenEarthCoordinates(lat1, lon1, lat2, lon2) {
  const earthRadiusKm = 6371;

  const dLat = degreesToRadians(lat2-lat1);
  const dLon = degreesToRadians(lon2-lon1);

  lat1 = degreesToRadians(lat1);
  lat2 = degreesToRadians(lat2);

  const a = Math.sin(dLat/2) * Math.sin(dLat/2) +
          Math.sin(dLon/2) * Math.sin(dLon/2) * Math.cos(lat1) * Math.cos(lat2);
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
  return earthRadiusKm * c;
}