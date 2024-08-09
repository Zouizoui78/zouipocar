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

    // Send fix to clients that have subscribed to /api/event/fix
    void send_fix(const Fix& fix);

private:
    Database* _db;
    httplib::Server svr;
    std::optional<Fix> _last_fix;

    // Objects used to synchronize sending fixes to clients
    std::condition_variable _cv;
    std::mutex _cvm;
    std::atomic_int _cvid = 0;
    std::atomic_int _cvcid = -1;

    void register_handlers();
    void api_fix(const httplib::Request &req, httplib::Response &res);
    void api_fix_first(const httplib::Request &req, httplib::Response &res);
    void api_fix_last(const httplib::Request &req, httplib::Response &res);
    void api_range(const httplib::Request &req, httplib::Response &res);
    void api_event_fix(const httplib::Request &req, httplib::Response &res);

    void wait_event_fix(httplib::DataSink& sink);
    std::optional<Fix> get_last_fix();
};

}

#endif // HTTPSERVER_HPP
