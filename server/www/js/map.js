class ZouipocarMap {
    map;
    #carMarker;
    #tracking = true;
    #userPanning = true;
    #markerSize = [60, 75];
    #markerAnchor = [30, 75];
    #defaultZoom = 16;
    #currentPos;

    constructor(
        pos = [0, 0],
        showCarMarker = false,
        enableTrackButton = false,
        divId = "map"
    ) {
        this.#currentPos = pos;

        const mapOptions = {
            center: this.#currentPos,
            zoom: this.#defaultZoom,
            tap: false,
            attributionControl: false
        }

        const tilesOptions = {
            detectRetina: true
        }

        if (utils.isClientMobile()) {
            // Do not force zoom to have fixed steps.
            mapOptions.zoomSnap = 0;
        }

        this.map = L.map(divId, mapOptions);
        new L.TileLayer('https://{s}.tile.openstreetmap.fr/osmfr/{z}/{x}/{y}.png', tilesOptions).addTo(this.map);

        if (showCarMarker) {
            this.#showCarMarker();
        }

        if (enableTrackButton) {
            this.#initTrackButton();
        }
    }

    setCarPos(pos) {
        this.#currentPos = pos;
        this.#carMarker.setLatLng(this.#currentPos);
        if (this.#tracking) {
            this.#userPanning = false;
            this.#center();
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

    #showCarMarker() {
        const carIcon = L.icon({
            iconUrl: 'images/car.png',
            iconSize: this.#markerSize,
            iconAnchor: this.#markerAnchor,
        });

        this.#carMarker = L.marker(this.#currentPos, { icon: carIcon }).addTo(this.map);
        this.#carMarker.on("click", e => { this.#makePopupContent(this.#currentPos); });
        this.#carMarker.bindPopup("");
    }

    #center(){
        const currentZoom = this.map.getZoom();
        if (currentZoom < this.#defaultZoom) {
            this.map.setView(this.#currentPos, this.#defaultZoom, { animate: false });
        }
        else {
            this.map.setView(this.#currentPos, currentZoom, { animate: false });
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

    #makePopupContent() {
        const popupContent = "Latitude = " +
            utils.formatNumber(this.#currentPos[0]) +
            "<br>Longitude = " +
            utils.formatNumber(this.#currentPos[1]);
        this.#carMarker.setPopupContent(popupContent);

        const url = 'https://nominatim.openstreetmap.org/reverse?format=jsonv2&lat=' + this.#currentPos[0] + '&lon=' + this.#currentPos[1];
        ajax.get(url, null, body => {
            const parsed = JSON.parse(body).address;
            if (!parsed) {
                return;
            }

            let town = parsed.town;
            if (!town) {
                town = parsed.village;
            }
            if (!town) {
                town = parsed.city;
            }

            let addr = parsed.road + ", " + parsed.postcode + " " + town + ", " + parsed.country;
            if (parsed.house_number) {
                addr = parsed.house_number + " " + addr;
            }
            this.#carMarker.setPopupContent(popupContent + "<br>" + addr);
        });
    }
}
