#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <memory>

#include "Fix.hpp"
#include "httplib.h"

namespace zouipocar {

class Database;

class HTTPServer : public httplib::Server {
public:
    HTTPServer(std::unique_ptr<Database>&& db);

private:
    std::unique_ptr<Database> _db;
    std::optional<Fix> _first_fix;

    void register_handlers();
    void api_fix(const httplib::Request &req, httplib::Response &res);
    void api_fix_first(const httplib::Request &req, httplib::Response &res);
    void api_fix_last(const httplib::Request &req, httplib::Response &res);
    void api_range(const httplib::Request &req, httplib::Response &res);
};

}

#endif // HTTPSERVER_HPP