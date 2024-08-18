let map;
let interval;

ajax.get(
    "api/fix/last",
    null,
    "arraybuffer",
    (req) => {
        const fix = utils.parseFix(req.response);
        const pos = [fix.latitude, fix.longitude];
        map = new ZouipocarMap(pos, true, true);
        setSpeed(fix.speed);
    },
    () => {
        map = new ZouipocarMap([0, 0], true, true);
    }
);

document.addEventListener("visibilitychange", () => {
    if (document.visibilityState == "visible") {
        getLastFix();
        interval = setInterval(getLastFix, 1000);
    }
    else {
        clearInterval(interval);
    }
});

interval = setInterval(getLastFix, 1000);

function getLastFix() {
    ajax.get("api/fix/last", null, "arraybuffer", (req) => {
        onFix(req.response);
    });
}

function onFix(buffer) {
    const fix = utils.parseFix(buffer);
    const pos = [fix.latitude, fix.longitude];

    console.log(fix);

    map.setCarPos(pos);
    setSpeed(fix.speed);
}

function setSpeed(speed) {
    if (speed < 5) {
        document.getElementById("speed").innerHTML = "0";
    } else {
        document.getElementById("speed").innerHTML = speed;
    }
}
