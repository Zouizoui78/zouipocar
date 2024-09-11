#include "HTTPServer.hpp"
#include "Database.hpp"
#include "ErrorMessages.hpp"

namespace zouipocar::http {

using namespace std::placeholders;
using namespace httplib;
using namespace zouipocar;

HTTPServer::HTTPServer(std::string_view web_ui_path, Database *db) : _db(db) {
    register_handlers();
    svr.set_mount_point("/", std::string(web_ui_path));

    _first_fix = _db->get_first_fix();
    std::lock_guard lock(_last_fix_mutex);
    _last_fix = _db->get_last_fix();
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
    std::lock_guard lock(_last_fix_mutex);
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
        res.set_content(ErrorMessages::api_fix_missing_parameter, "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    std::string date_param = req.get_param_value("date");
    uint32_t date = 0;
    try {
        date = std::stoul(date_param);
    }
    catch (std::invalid_argument &e) {
        res.set_content(ErrorMessages::api_fix_invalid_parameter, "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }
    catch (std::out_of_range &e) {
        res.set_content(ErrorMessages::api_fix_out_of_range_parameter,
                        "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    auto fix = _db->get_fix(date);
    if (!fix.has_value()) {
        res.status = StatusCode::NotFound_404;
        return;
    }
    res.set_content(reinterpret_cast<const char *>(&*fix), sizeof(Fix),
                    "application/octet-stream");
}

void HTTPServer::api_fix_first(const Request &req, Response &res) {
    if (!_first_fix.has_value()) [[unlikely]] {
        res.status = StatusCode::NotFound_404;
        return;
    }

    res.set_content(reinterpret_cast<const char *>(&*_first_fix), sizeof(Fix),
                    "application/octet-stream");
}

void HTTPServer::api_fix_last(const Request &req, Response &res) {
    std::lock_guard lock(_last_fix_mutex);

    if (!_last_fix.has_value()) [[unlikely]] {
        res.status = StatusCode::NotFound_404;
        return;
    }

    res.set_content(reinterpret_cast<const char *>(&*_last_fix), sizeof(Fix),
                    "application/octet-stream");
}

void HTTPServer::api_range(const Request &req, Response &res) {
    if (!req.has_param("start") || !req.has_param("stop")) {
        res.set_content(ErrorMessages::api_range_missing_parameter,
                        "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    uint32_t start = 0;
    uint32_t stop = 0;
    try {
        start = std::stoul(req.get_param_value("start"));
        stop = std::stoul(req.get_param_value("stop"));
    }
    catch (std::invalid_argument &e) {
        res.set_content(ErrorMessages::api_range_invalid_parameter,
                        "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }
    catch (std::out_of_range &e) {
        res.set_content(ErrorMessages::api_range_out_of_range_parameter,
                        "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    if (start >= stop) {
        res.set_content(ErrorMessages::api_range_invalid_range, "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    auto fixes = _db->get_fix_range(start, stop);
    res.set_content(reinterpret_cast<const char *>(fixes.data()),
                    sizeof(Fix) * fixes.size(), "application/octet-stream");
}

} // namespace zouipocar::http
