#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <condition_variable>

#include "Fix.hpp"
#include "httplib.h"

namespace zouipocar {

class Database;

class HTTPServer {
public:
    // Doesn't take owernship of db.
    HTTPServer(std::string_view web_ui_path, Database* db);

    bool listen(const std::string& addr, int port);
    void stop();

    // Answer pending requests to /api/pollfix with the fix
    void update_last_fix(const Fix& fix);

private:
    Database* _db;
    httplib::Server svr;
    Fix _last_fix;

    // Objects used to synchronize responses to requests to /api/pollfix
    std::condition_variable _cv;
    std::mutex _cvm;
    bool _cv_ready = false;
    bool _cv_stopping = false;

    void register_handlers();
    void api_fix(const httplib::Request &req, httplib::Response &res);
    void api_fix_first(const httplib::Request &req, httplib::Response &res);
    void api_fix_last(const httplib::Request &req, httplib::Response &res);
    void api_range(const httplib::Request &req, httplib::Response &res);
    void api_poll_fix(const httplib::Request &req, httplib::Response &res);
};

}

#endif // HTTPSERVER_HPP
