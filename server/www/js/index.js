let map;

ajax.get("api/fix/last", null,
    (json_fix) => {
        const fix = JSON.parse(json_fix);
        const pos = [
            fix.latitude,
            fix.longitude
        ];

        map = new ZouipocarMap(pos, true, true);
        setSpeed(fix.speed);
    },
    () => {
        map = new ZouipocarMap([0, 0], true, true);
    }
);

document.addEventListener("visibilitychange", () => {
    if (document.visibilityState == "visible") {
        ajax.get("api/fix/last", null, (json_fix) => { onFix(json_fix); });
    }
});

const ev = new EventSource("api/event/fix");
ev.onmessage = function(e) {
    onFix(e.data);
}

function onFix(json_fix) {
    const fix = JSON.parse(json_fix);
    const pos = [
        fix.latitude,
        fix.longitude
    ];

    console.log(fix);

    map.setCarPos(pos);
    setSpeed(fix.speed);
}

function setSpeed(speed) {
    if (speed < 5){
        document.getElementById('speed').innerHTML = '0';
    }
    else{
        document.getElementById('speed').innerHTML = speed;
    }
}