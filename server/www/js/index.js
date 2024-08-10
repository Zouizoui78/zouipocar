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
    zoom: defaultZoom,
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

getLastFix();

const ev = new EventSource("api/event/fix");
ev.onmessage = function(e) {
    onFix(e.data);
}

document.addEventListener("visibilitychange", () => {
    if (document.visibilityState == "visible") {
        getLastFix();
    }
});

function getLastFix() {
    ajax.get("/api/fix/last", null, onFix);
}

function onFix(json_fix) {
    fix = JSON.parse(json_fix);
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
        if (utils.mobileCheck())
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

function makePopupContent(marker, pos) {
    let popupContent = "Latitude = " +
        utils.formatNumber(pos[0]) +
        "<br>Longitude = " +
        utils.formatNumber(pos[1]);
    marker.setPopupContent(popupContent);

    url = 'https://nominatim.openstreetmap.org/reverse?format=jsonv2&lat=' + pos[0] + '&lon=' + pos[1];
    ajax.get(url, null, body => {
        let parsed = JSON.parse(body).address;
        let addr = parsed.road + ", " + parsed.postcode + " " + parsed.town + ", " + parsed.country;
        if (parsed.house_number) {
            addr = parsed.house_number + " " + addr;
        }
        marker.setPopupContent(popupContent + "<br>" + addr);
    });
}
