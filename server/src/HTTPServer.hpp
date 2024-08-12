#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>

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

    void send_fix_event(const fix::Fix& fix);

private:
    Database* _db;
    httplib::Server svr;
    std::optional<fix::Fix> _last_fix;

    // Objects used to synchronize sending fixes to clients.
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

    // Return false if sink is not writable or if there is no data to send.
    bool wait_event_fix(httplib::DataSink& sink);

    // Return _last_fix if it has a value, Database::get_last_fix() otherwise.
    std::optional<fix::Fix> get_last_fix();
};

}

#endif
