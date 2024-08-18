#include "HTTPServer.hpp"
#include "Database.hpp"

namespace zouipocar {

using namespace std::placeholders;
using namespace httplib;

std::string serialize_fix(const Fix &fix) {
    return std::string(reinterpret_cast<const char *>(&fix), sizeof(Fix));
}

std::string serialize_fixes(const std::vector<Fix> &fixes) {
    return std::string(reinterpret_cast<const char *>(fixes.data()),
                       sizeof(Fix) * fixes.size());
}

HTTPServer::HTTPServer(std::string_view web_ui_path, Database *db) : _db(db) {
    register_handlers();
    svr.set_mount_point("/", std::string(web_ui_path));
}

bool HTTPServer::listen(const std::string &addr, int port) {
    return svr.listen(addr, port);
}

void HTTPServer::wait_until_ready() {
    svr.wait_until_ready();
}

void HTTPServer::stop() {
    svr.stop();
}

void HTTPServer::update_fix(const Fix &fix) {
    _last_fix = fix;
}

void HTTPServer::register_handlers() {
    svr.Get("/api/fix/first",
            std::bind(&HTTPServer::api_fix_first, this, _1, _2));
    svr.Get("/api/fix/last",
            std::bind(&HTTPServer::api_fix_last, this, _1, _2));
    svr.Get("/api/fix", std::bind(&HTTPServer::api_fix, this, _1, _2));
    svr.Get("/api/range", std::bind(&HTTPServer::api_range, this, _1, _2));
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
    res.set_content(serialize_fix(*fix), "application/octet-stream");
}

void HTTPServer::api_fix_first(const Request &req, Response &res) {
    auto fix = _db->get_first_fix();
    if (!fix.has_value()) {
        res.status = StatusCode::NotFound_404;
        return;
    }
    res.set_content(serialize_fix(*fix), "application/octet-stream");
}

void HTTPServer::api_fix_last(const Request &req, Response &res) {
    auto fix = _last_fix.has_value() ? _last_fix : _db->get_last_fix();
    if (!fix.has_value()) {
        res.status = StatusCode::NotFound_404;
        return;
    }
    res.set_content(serialize_fix(*fix), "application/octet-stream");
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
        res.set_content("Out of range start and/or stop parameter",
                        "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    if (start >= stop) {
        res.set_content(
            "start parameter must be strictly less than stop parameter",
            "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    res.set_content(serialize_fixes(_db->get_fix_range(start, stop)),
                    "application/octet-stream");
}

} // namespace zouipocar
