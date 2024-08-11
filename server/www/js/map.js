class ZouipocarMap {
    map;
    #carMarker;
    #tracking = true;
    #userPanning = true;
    #markerSize = [60, 75];
    #markerAnchor = [30, 75];
    #defaultZoom = 16;

    constructor(
        pos = [0, 0],
        showCarMarker = false,
        enableTrackButton = false,
        divId = "map"
    ) {
        const mapOptions = {
            center: pos,
            zoom: this.#defaultZoom,
            tap: false,
            attributionControl: false
        }

        const tilesOptions = {}

        if (utils.isClientMobile()) {
            mapOptions.zoomSnap = 0;
            tilesOptions.tileSize = 512;
            tilesOptions.zoomOffset = -1;
        }

        this.map = L.map(divId, mapOptions);
        new L.TileLayer('https://{s}.tile.openstreetmap.fr/osmfr/{z}/{x}/{y}.png', tilesOptions).addTo(this.map);

        if (showCarMarker) {
            this.#showCarMarker(pos);
        }

        if (enableTrackButton) {
            this.#initTrackButton();
        }
    }

    setCarPos(pos) {
        this.#carMarker.setLatLng(pos);
        if (this.#tracking) {
            this.#userPanning = false;
            this.#center(pos);
            this.#userPanning = true;
        }
    }

    showHouseMarker(housePos) {
        const houseIcon = L.icon({
            iconUrl: 'images/house.png',
            iconSize: this.#markerSize,
            iconAnchor: this.#markerAnchor,
        });
        L.marker(housePos, { icon: houseIcon }).addTo(this.map);
    }

    #showCarMarker(pos) {
        const carIcon = L.icon({
            iconUrl: 'images/car.png',
            iconSize: this.#markerSize,
            iconAnchor: this.#markerAnchor,
        });

        let popupOptions = {};
        if (utils.isClientMobile()) {
            popupOptions = { 'className': 'bigger-popup' };
        }

        this.#carMarker = L.marker(pos, { icon: carIcon }).addTo(this.map);
        this.#carMarker.on("click", e => { this.#makePopupContent(pos); });
        this.#carMarker.bindPopup("", popupOptions);
    }

    #center(pos){
        const currentZoom = this.map.getZoom();
        if (currentZoom < this.#defaultZoom) {
            this.map.setView(pos, this.#defaultZoom, { animate: false });
        }
        else {
            this.map.setView(pos, currentZoom, { animate: false });
        }
    }

    #initTrackButton() {
        const trackButton = document.getElementById('trackButton');
        trackButton.disabled = true;

        trackButton.addEventListener('click', () => {
            this.#tracking = true;
            this.#userPanning = false;
            this.#center(this.#carMarker.getLatLng());
            this.#userPanning = true;
            trackButton.disabled = true;
            trackButton.src = "images/location_black.png";
        });

        this.map.on('movestart', e => {
            if (this.#userPanning) {
                this.#tracking = false;
                trackButton.disabled = false;
                trackButton.src = "images/location_gray.png";
            }
        });
    }

    #makePopupContent(pos) {
        const popupContent = "Latitude = " +
            utils.formatNumber(pos[0]) +
            "<br>Longitude = " +
            utils.formatNumber(pos[1]);
        this.#carMarker.setPopupContent(popupContent);

        const url = 'https://nominatim.openstreetmap.org/reverse?format=jsonv2&lat=' + pos[0] + '&lon=' + pos[1];
        ajax.get(url, null, body => {
            const parsed = JSON.parse(body).address;
            if (!parsed) {
                return;
            }

            let town = parsed.town;
            if (!town) {
                town = parsed.village;
            }

            let addr = parsed.road + ", " + parsed.postcode + " " + town + ", " + parsed.country;
            if (parsed.house_number) {
                addr = parsed.house_number + " " + addr;
            }
            this.#carMarker.setPopupContent(popupContent + "<br>" + addr);
        });
    }
}
