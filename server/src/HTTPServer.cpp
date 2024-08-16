#include "Database.hpp"
#include "HTTPServer.hpp"

namespace zouipocar {

using namespace std::placeholders;
using namespace httplib;

using json = nlohmann::json;

HTTPServer::HTTPServer(std::string_view web_ui_path, Database* db)
    : _db(db)
{
    register_handlers();
    svr.set_mount_point("/", std::string(web_ui_path));
}

bool HTTPServer::listen(const std::string& addr, int port) {
    return svr.listen(addr, port);
}

void HTTPServer::wait_until_ready() {
    svr.wait_until_ready();
}

void HTTPServer::stop() {
    std::lock_guard lock(_cvm);
    _cvcid = _cvid.load();
    _cv.notify_all();
    svr.stop();
}

void HTTPServer::send_fix_event(const Fix& fix) {
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
        res.set_content("Missing start and/or stop parameter", "text/plain");
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
        res.set_content("Invalid start and/or stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }
    catch (std::out_of_range &e) {
        res.set_content("Out of range start and/or stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    if (start >= stop) {
        res.set_content("start parameter must be strictly less than stop parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    res.set_content(json(_db->get_fix_range(start, stop)).dump(), "application/json");
}

void HTTPServer::api_event_fix(const httplib::Request &req, httplib::Response &res) {
    // These two headers should ensure that no buffering gets in the stream's way.
    // https://serverfault.com/a/801629
    res.set_header("X-Accel-Buffering", "no");
    res.set_header("Cache-Control", "no-cache");
    res.set_chunked_content_provider(
        "text/event-stream",
        [this](size_t size, DataSink& sink) {
            // Stream is canceled if this returns false.
            return wait_event_fix(sink);
        }
    );
}

bool HTTPServer::wait_event_fix(DataSink& sink) {
    std::unique_lock lock(_cvm);
    int id = _cvid;

    // We have to check whether the stream is writable here,
    // otherwise the thread would hang forever
    // after the client has disconnected.
    while (id != _cvcid && sink.is_writable()) {
        _cv.wait_for(lock, std::chrono::seconds(1));
    }

    if (!sink.is_writable()) {
        return false;
    }

    auto fix = get_last_fix();
    if (!fix.has_value()) {
        return false;
    }

    // The "server-sent events" spec says that an unnamed event message
    // must start with "data:" and that any event must end with an empty line
    // for the client to handle it.
    // This is to allow messages with multiple lines.
    sink.os << "data:" << json(fix.value()).dump() << "\n\n";
    return true;
}

std::optional<Fix> HTTPServer::get_last_fix() {
    return _last_fix.has_value() ? _last_fix : _db->get_last_fix();
}

}
