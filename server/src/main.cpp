// #include "ws/server_ws.hpp"
#include "json.hpp"

#include "Database.hpp"
#include "HTTPServer.hpp"
#include "tools.hpp"
#include "UDP.hpp"

#include <csignal>

using json = nlohmann::json;

// using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

#define HTTP_PORT 3001
#define UDP_PORT 3002
#define WEBSOCKET_PORT 3003

#define PACKET_SIZE 15


std::function<void (int)> handler;
void signal_handler_caller(int signal) {
    std::cout << "Received signal " << strsignal(signal) << std::endl;
    handler(signal);
}

int main(void)
{
    std::cout << "Program start\n";

    signal(SIGINT, signal_handler_caller);
    signal(SIGTERM, signal_handler_caller);

    std::string db_path = "zouicar.sqlite";
    Database db(db_path);
    if (!db.is_open()) {
        std::cout << "Failed to open database\n";
        return 1;
    }

    // Default fix json
    json last_fix = db.query_last_fix();

    // Websocket setup    
    // WsServer ws_server;
    // ws_server.config.port = WEBSOCKET_PORT;
    
    // auto &echo = ws_server.endpoint["/"];

    // echo.on_open = [&last_fix](std::shared_ptr<WsServer::Connection> connection) {
    //     json j = {
    //         { "id", "fix" },
    //         { "data", last_fix }
    //     };
    //     connection->send(j.dump());
    // };

    #ifdef DEBUG
    uint32_t count = 0;
    #endif

    // UDP listener setup
    UDP udp(UDP_PORT);
    udp.start_listen([&](uint8_t *data, size_t size) {
        if (size != PACKET_SIZE) {
            std::cout << "Received invalid data.\n";
            return;
        }

        json j = {
            { "id", "fix" },
            { "data", parse_packet(data) }
        };

        if (j["data"]["timestamp"] <= last_fix["timestamp"])
            return;

        // for (const auto &e : ws_server.get_connections()) {
        //     e->send(j.dump());
        // }

        if (j["data"]["speed"] >= 5) {
            db.insert_fix(j["data"]);
            #ifdef DEBUG
            std::cout << ++count << std::endl;
            #endif
        }

        last_fix = j["data"];
    });

    // Start servers
    // std::thread ws_thread([&ws_server]() {
    //     ws_server.start([](unsigned short port) {
    //         std::cout << "Websocket server listening on port " << port << std::endl;
    //     });
    // });

    HTTPServer http(db_path);
    std::thread http_thread([&http]() {
        http.listen("0.0.0.0", HTTP_PORT);
    });

    // Signal handler
    handler = [&](int signal) {
        if (signal == SIGINT || signal == SIGTERM) {
            // ws_server.stop();
            http.stop();
        }
    };
    
    // Stop program
    // ws_thread.join();
    std::cout << "Stopped websocket server\n";

    http_thread.join();
    std::cout << "Stopped http server\n";

    return 0;
}