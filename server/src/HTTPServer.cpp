#include "HTTPServer.hpp"

using namespace std::placeholders;

HTTPServer::HTTPServer(const std::string &db_path) :
    httplib::Server(),
    db(db_path)
{
    register_handlers();
    this->set_mount_point("/", "./www");
    _first_fix = db.query_first_fix();
}

HTTPServer::~HTTPServer() {}

void HTTPServer::register_handlers() {
    this->Get("/api/fix", std::bind(&HTTPServer::api_fix, this, _1, _2));
    this->Get("/api/range", std::bind(&HTTPServer::api_range, this, _1, _2));

    std::cout << "Registered HTTP server handlers\n";
}

void HTTPServer::api_fix(const httplib::Request &req, httplib::Response &res) {
    if (req.has_param("date")) {
        std::string date_param = req.get_param_value("date");
        if (date_param == "first") {
            res.set_content(_first_fix.dump(), "application/json");
        }
        else if (date_param == "last") {
            res.set_content(db.query_last_fix().dump(), "application/json");
        }
        else {
            try {
                time_t date = std::stol(date_param);
                res.set_content(db.query_fix(date).dump(), "application/json");
            }
            catch (std::invalid_argument &e) {
                res.status = 400;
            }
        }
    }
    else {
        res.status = 400;
    }
}

void HTTPServer::api_range(const httplib::Request &req, httplib::Response &res) {
    if (req.has_param("start") && req.has_param("stop")) {
        try {
            time_t start = std::stol(req.get_param_value("start"));
            time_t stop = std::stol(req.get_param_value("stop"));

            if (start >= stop)
                res.status = 400;
            else
                res.set_content(db.query_fix_range(start, stop).dump(), "application/json");
        }
        catch (std::invalid_argument &e) {
            res.status = 400;
        }
    }
    else {
        res.status = 400;
    }
}