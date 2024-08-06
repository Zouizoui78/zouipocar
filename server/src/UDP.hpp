#ifndef UDP_HPP
#define UDP_HPP

#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <functional>
#include <thread>

namespace zouipocar {

class UDP {
public:
    using UDPCallback = std::function<void (std::vector<uint8_t>)>;

    UDP(uint16_t port, UDPCallback callback);
    ~UDP();

    UDP(const UDP& other) = delete;
    UDP(UDP&& other) = delete;
    UDP& operator=(const UDP& other) = delete;
    UDP& operator=(UDP&& other) = delete;

private:
    void listen(UDPCallback callback);

    int _socket;
    std::jthread _listen_thread;
    bool _listen_thread_running = false;
    std::vector<uint8_t> _listen_buffer;
};

}

#endif // UDP