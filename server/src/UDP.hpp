#ifndef UDP_HPP
#define UDP_HPP

#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <functional>
#include <thread>

#define UDP_MAX_PACKET_SIZE 65527

class UDP {
    public:
    
    UDP(uint16_t port);

    ~UDP();

    bool is_ok();

    bool start_listen(std::function<void (uint8_t *data, size_t size)> callback);

    private:
    int _socket;
    sockaddr_in _addr;

    bool _ok = false;

    std::thread _listen_thread;
    bool _listen_thread_running = false;
    std::function<void (uint8_t *data, size_t size)> _listen_callback;
    uint8_t *_listen_buffer = nullptr;
};

#endif // UDP