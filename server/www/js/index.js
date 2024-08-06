let map, layer, marker, userMarker;
let house = [43.6007822,1.4730307];
let pos = [];
let initDone = false;
let auto = true;
let userPan = true;
let defaultZoom = 16;
let markerWidth = 60;
let markerHeight = markerWidth * 5/4;

//Center button setup
let centerButton = document.getElementById('centerButton');
centerButton.disabled = true;
centerButton.addEventListener('click', () => {
    auto = true;
    userPan = false;
    center(map, pos, defaultZoom);
    userPan = true;
    disableButton(centerButton);
});

// Map setup
mapOptions = {
    attributionControl: false,
    center: house,
    zoom: defaultZoom,
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

var houseIcon = L.icon({
    iconUrl: '/images/house.png',
    iconSize:     [markerWidth, markerHeight],
    iconAnchor:   [markerWidth / 2, markerHeight],
});
L.marker(house, { icon: houseIcon }).addTo(map);

map.on('movestart', e => {
    if(userPan){
        auto = false;
        enableButton(centerButton);
    }
});

let s = connect();
setInterval(() => {
    if (s.readyState != WebSocket.CONNECTING && s.readyState != WebSocket.OPEN) {
        s = connect();
    }
}, 1000);

function connect() {
    //Socket setup
    let socket = new WebSocket("wss://example.com/websocket");

    socket.onmessage = e => {
        j = JSON.parse(e.data);

        if (j.id != "fix") {
            return;
        }

        fix = j.data;
        console.log(fix);

        pos = [
            fix.latitude,
            fix.longitude
        ];

        if (!initDone) {
            var car = L.icon({
                iconUrl: '/images/car.png',
                iconSize:     [markerWidth, markerHeight],
                iconAnchor:   [markerWidth / 2, markerHeight],
            });

            // Car marker setup
            popupOptions = {};
            if (mobileCheck())
                popupOptions = { 'className': 'bigger-popup' };
            marker = L.marker(pos, { icon: car }).addTo(map)
                .on("click", e => {
                    makePopupContent(marker, pos);
                })
                .bindPopup("", popupOptions);

            initDone = true;
        }

        if(auto){
            userPan = false;
            center(map, pos, defaultZoom);
            userPan = true;
        }

        marker.setLatLng(pos);

        if(fix.speed < 5){
            document.getElementById('speed').innerHTML = '0';
        }
        else{
            document.getElementById('speed').innerHTML = fix.speed;
        }
    };

    return socket;
}

function updateUserMarker(marker) {
    navigator.geolocation.getCurrentPosition(e => {
        marker.setLatLng([e.coords.latitude, e.coords.longitude]);
    });
}

function center(map, pos, defaultZoom){
    if(map.getZoom() < defaultZoom){
        map.setView(pos, defaultZoom, { animate: false });
    }
    else{
        map.setView(pos, map.getZoom(), { animate: false });
    }
}

function enableButton(button){
    button.disabled = false;
    button.src = "/images/location_gray.png";
}

function disableButton(button){
    button.disabled = true;
    button.src = "/images/location_black.png";
}

function formatNumber(number) {
    return Number.parseFloat(number).toFixed(5);
}

function makePopupContent(marker, pos) {
    let popupContent = "Latitude = " + formatNumber(pos[0]) + "<br>Longitude = " + formatNumber(pos[1]);
    marker.setPopupContent(popupContent);

    url = 'https://nominatim.openstreetmap.org/reverse?format=jsonv2&lat=' + pos[0] + '&lon=' + pos[1];
    var request = new XMLHttpRequest();
    request.open('GET', url);  // `false` makes the request synchronous
    request.send();

    request.onreadystatechange = e => {
        if (request.readyState == 4 && request.status == 200) {
            let parsed = JSON.parse(request.responseText).address;
            let addr = parsed.house_number + " " + parsed.road + ", " + parsed.postcode + " " + parsed.city;
            if (addr != null) {
                marker.setPopupContent(popupContent + "<br>" + addr);
            }
        }
    }
}

function mobileCheck() {
    let check = false;
    (function(a){if(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|mobile.+firefox|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows ce|xda|xiino/i.test(a)||/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(a.substr(0,4))) check = true;})(navigator.userAgent||navigator.vendor||window.opera);
    return check;
};
