#include "Database.hpp"
#include "HTTPServer.hpp"
#include "json.hpp"

using namespace std::placeholders;
using namespace httplib;

namespace zouipocar {

using json = nlohmann::json;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Fix, timestamp, speed, latitude, longitude);

HTTPServer::HTTPServer(std::string_view web_ui_path, Database* db)
    : _db(db)
{
    register_handlers();
    svr.set_mount_point("/", std::string(web_ui_path));
}

bool HTTPServer::listen(const std::string& addr, int port) {
    return svr.listen(addr, port);
}

void HTTPServer::stop() {
    std::lock_guard lock(_cvm);
    _cvcid = _cvid.load();
    _cv.notify_all();
    svr.stop();
}

void HTTPServer::send_fix(const Fix& fix) {
    std::lock_guard lock(_cvm);
    _cvcid = _cvid++;
    _last_fix = fix;
    _cv.notify_all();
}

void HTTPServer::register_handlers() {
    svr.Get("/api/fix/first", std::bind(&HTTPServer::api_fix_first, this, _1, _2));
    svr.Get("/api/fix/last", std::bind(&HTTPServer::api_fix_last, this, _1, _2));
    svr.Get("/api/fix", std::bind(&HTTPServer::api_fix, this, _1, _2));
    svr.Get("/api/range", std::bind(&HTTPServer::api_range, this, _1, _2));
    svr.Get("/api/event/fix", std::bind(&HTTPServer::api_event_fix, this, _1, _2));
}

void HTTPServer::api_fix(const Request &req, Response &res) {
    if (!req.has_param("date")) {
        res.set_content("Missing date parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    std::string date_param = req.get_param_value("date");
    uint32_t date = 0;
    try {
        date = std::stol(date_param);
    }
    catch (std::invalid_argument &e) {
        res.set_content("Invalid date parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }
    catch (std::out_of_range &e) {
        res.set_content("Out of range date parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    auto fix = _db->get_fix(date);
    if (!fix.has_value()) {
        res.status = StatusCode::NotFound_404;
        return;
    }
    res.set_content(json(*fix).dump(), "application/json");
}

void HTTPServer::api_fix_first(const Request &req, Response &res) {
    auto fix = _db->get_first_fix();
    if (!fix.has_value()) {
        res.status = StatusCode::NotFound_404;
        return;
    }
    res.set_content(json(*fix).dump(), "application/json");
}

void HTTPServer::api_fix_last(const Request &req, Response &res) {
    auto fix = get_last_fix();
    if (!fix.has_value()) {
        res.status = StatusCode::NotFound_404;
        return;
    }
    res.set_content(json(*fix).dump(), "application/json");
}

void HTTPServer::api_range(const Request &req, Response &res) {
    if (!req.has_param("start") || !req.has_param("stop")) {
        res.set_content("Missing range's start and/or stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    uint32_t start = 0;
    uint32_t stop = 0;
    try {
        start = std::stol(req.get_param_value("start"));
        stop = std::stol(req.get_param_value("stop"));
    }
    catch (std::invalid_argument &e) {
        res.set_content("Invalid range's start and/or stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }
    catch (std::out_of_range &e) {
        res.set_content("Out of range range's start and/or stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    if (start >= stop) {
        res.set_content("start parameter cannot be greater than stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    res.set_content(json(_db->get_fix_range(start, stop)).dump(), "application/json");
}

void HTTPServer::api_event_fix(const httplib::Request &req, httplib::Response &res) {
    // https://serverfault.com/a/801629
    res.set_header("X-Accel-Buffering", "no"); // Disable response buffering
    res.set_header("Cache-Control", "no-cache"); // Disable response caching
    res.set_chunked_content_provider(
        "text/event-stream",
        [this](size_t size, DataSink& sink) {
            wait_event_fix(sink);
            return true;
        }
    );
}

void HTTPServer::wait_event_fix(DataSink& sink) {
    std::unique_lock lock(_cvm);
    int id = _cvid;

    while (id != _cvcid && sink.is_writable()) {
        _cv.wait_for(lock, std::chrono::seconds(1));
    }

    if (!sink.is_writable()) {
        return;
    }

    auto fix = get_last_fix();
    if (!fix.has_value()) {
        return;
    }

    std::string msg = "data: " + json(fix.value()).dump() + "\n\n";
    sink.write(msg.data(), msg.size());
}

std::optional<Fix> HTTPServer::get_last_fix() {
    return _last_fix.has_value() ? _last_fix : _db->get_last_fix();
}

}
