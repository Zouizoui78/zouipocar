var ajax = {};

ajax.x = function () {
    if (typeof XMLHttpRequest !== 'undefined') {
        return new XMLHttpRequest();
    }
    var versions = [
        "MSXML2.XmlHttp.6.0", "MSXML2.XmlHttp.5.0", "MSXML2.XmlHttp.4.0",
        "MSXML2.XmlHttp.3.0", "MSXML2.XmlHttp.2.0", "Microsoft.XmlHttp"
    ];

    var xhr;
    for (var i = 0; i < versions.length; i++) {
        try {
            xhr = new ActiveXObject(versions[i]);
            break;
        }
        catch (e) {
        }
    }
    return xhr;
};

ajax.send = function (url, method, data, responseType, onSuccess, onError, onEnd) {
    if (url[0] == '/')
        url = window.location.origin + url;

    var x = ajax.x();
    x.open(method, url);
    x.addEventListener("error", onError);
    x.onreadystatechange = function () {
        if (x.readyState == XMLHttpRequest.DONE) {
            if (x.status >= 200 && x.status < 300) {
                if (onSuccess)
                    onSuccess(x);
            }
            else if (x.status >= 400) {
                if (onError)
                    onError(x);
            }

            if (onEnd) {
                onEnd(x);
            }
        }
    };
    if (method == 'POST' || method == 'PUT')
        x.setRequestHeader('Content-type', 'application/json');

    if (!data)
        data = "";

    if (responseType) {
        x.responseType = responseType;
    }

    x.send(data);
};

ajax.get = function (url, data, responseType, onSuccess, onError, onEnd) {
    ajax.send(url, 'GET', null, responseType, onSuccess, onError, onEnd);
};