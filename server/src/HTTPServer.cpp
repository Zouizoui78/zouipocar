#include "Database.hpp"
#include "HTTPServer.hpp"
#include "json.hpp"

using namespace std::placeholders;
using namespace httplib;

namespace zouipocar {

using json = nlohmann::json;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Fix, timestamp, speed, latitude, longitude);

HTTPServer::HTTPServer(std::string_view web_ui_path, Database* db) :
    Server(),
    _db(db)
{
    register_handlers();
    this->set_mount_point("/", std::string(web_ui_path));
}

void HTTPServer::update_last_fix(const Fix& fix) {
    {
        std::lock_guard lock(_cvm);
        _last_fix = fix;
        _cv_ready = true;
    }
    _cv.notify_all();

    // Wait for the request handlers to acknowledge the notify
    // by releasing the mutex in their threads.
    std::lock_guard lock(_cvm);
    _cv_ready = false;
}

void HTTPServer::register_handlers() {
    this->Get("/api/fix/first", std::bind(&HTTPServer::api_fix_first, this, _1, _2));
    this->Get("/api/fix/last", std::bind(&HTTPServer::api_fix_last, this, _1, _2));
    this->Get("/api/fix", std::bind(&HTTPServer::api_fix, this, _1, _2));
    this->Get("/api/range", std::bind(&HTTPServer::api_range, this, _1, _2));
    this->Get("/api/pollfix", std::bind(&HTTPServer::api_poll_fix, this, _1, _2));
}

void HTTPServer::api_fix(const Request &req, Response &res) {
    if (!req.has_param("date")) {
        res.set_content("Missing date parameter", "text/plain");
        res.status = StatusCode::BadRequest_400;
        return;
    }

    std::string date_param = req.get_param_value("date");
    time_t date = 0;
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
    auto fix = _db->get_last_fix();
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

    time_t start = 0;
    time_t stop = 0;
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

    std::vector<Fix> fixes = _db->get_fix_range(start, stop);

    if (fixes.empty()) {
        res.status = StatusCode::NotFound_404;
        return;
    }

    res.set_content(json(_db->get_fix_range(start, stop)).dump(), "application/json");
}

void HTTPServer::api_poll_fix(const Request& req, Response& res) {
    std::unique_lock lock(_cvm);
    _cv.wait(lock, [this]{ return _cv_ready; });
    lock.unlock();
    res.set_content(json(_last_fix).dump(), "application/json");
}

}
