#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

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
    HTTPServer(std::string_view web_ui_path, Database *db);

    bool listen(const std::string &addr, int port);
    void wait_until_ready();
    void stop();

    void update_fix(const Fix &fix);

private:
    Database *_db;
    httplib::Server svr;
    std::optional<Fix> _last_fix;
    std::mutex _last_fix_mutex;

    void register_handlers();
    void api_fix(const httplib::Request &req, httplib::Response &res);
    void api_fix_first(const httplib::Request &req, httplib::Response &res);
    void api_fix_last(const httplib::Request &req, httplib::Response &res);
    void api_range(const httplib::Request &req, httplib::Response &res);
};

} // namespace zouipocar

#endif
