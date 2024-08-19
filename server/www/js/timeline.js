const map = new ZouipocarMap();
let line;

const elem = document.querySelector('input[name="date"]');
const datepicker = new Datepicker(elem);

elem.addEventListener("changeDate", e => {
    const start = e.detail.date.getTime() / 1000;
    const stop = start + 86399;
    console.log(start, stop);
    ajax.get('api/range?start=' + start + '&stop=' + stop, null, "arraybuffer", update);
});

function update(req) {
    const buffer = req.response;
    let fixes = [];

    const fixSize = 13;
    for (i = 0; i < buffer.byteLength; i += fixSize) {
        fixes.push(utils.parseFix(buffer.slice(i, i + fixSize)));
    }

    if (fixes.length == 0)
        return;

    console.log("number of fixes = " + fixes.length);
    const polypoints = [];
    let distance = 0;
    let time = 0;
    console.log("first timestamp = " + fixes[0].timestamp);
    console.log("last timestamp = " + fixes[fixes.length - 1].timestamp);
    for (k in fixes) {
        if (k > 0) {
            distance += distanceInKmBetweenEarthCoordinates(
                fixes[k - 1].latitude, fixes[k - 1].longitude,
                fixes[k].latitude, fixes[k].longitude);
            const time_diff = fixes[k].timestamp - fixes[k - 1].timestamp;
            if (time_diff < 600)
                time += time_diff;
            else
                ++time;
        }
        const q = fixes[k];
        polypoints.push([q.latitude, q.longitude]);
    }

    const speed = distance / time * 3600;
    time = new Date(time * 1000);

    document.getElementById("distance").innerHTML = distance.toFixed(2);
    document.getElementById("duration").innerHTML =
        time.toISOString().split("T")[1].split(".")[0];
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

    const dLat = degreesToRadians(lat2 - lat1);
    const dLon = degreesToRadians(lon2 - lon1);

    lat1 = degreesToRadians(lat1);
    lat2 = degreesToRadians(lat2);

    const a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
        Math.sin(dLon / 2) * Math.sin(dLon / 2) * Math.cos(lat1) *
        Math.cos(lat2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    return earthRadiusKm * c;
}