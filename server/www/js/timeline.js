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

if (mobileCheck()) {
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

    url = 'https://example.com/api/range?start=' + start + '&stop=' + stop;
    var request = new XMLHttpRequest();
    request.open('GET', url);  // `false` makes the request synchronous
    request.send();

    request.onreadystatechange = e => {
        if (request.readyState == 4 && request.status == 200) {
            query = JSON.parse(request.responseText);
            if (!query)
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
    }
});

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

function mobileCheck() {
    let check = false;
    (function(a){if(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|mobile.+firefox|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows ce|xda|xiino/i.test(a)||/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(a.substr(0,4))) check = true;})(navigator.userAgent||navigator.vendor||window.opera);
    return check;
};
