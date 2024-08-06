#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "Database.hpp"
#include "httplib.h"

class HTTPServer : public httplib::Server {
    public:
    
    HTTPServer(const std::string &db_path);
    ~HTTPServer();

    private:

    void register_handlers();

    void api_fix(const httplib::Request &req, httplib::Response &res);
    void api_range(const httplib::Request &req, httplib::Response &res);

    Database db;
    json _first_fix;
};

#endif // HTTPSERVER_HPP