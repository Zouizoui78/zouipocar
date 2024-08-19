#ifndef UDP_HPP
#define UDP_HPP

#include <cstdint>
#include <functional>
#include <thread>

#include "Fix.hpp"

namespace zouipocar {

class UDP {
public:
    using UDPCallback = std::function<void(const Fix &)>;

    UDP(uint16_t port, UDPCallback callback);
    ~UDP();

    UDP(const UDP &other) = delete;
    UDP(UDP &&other) = delete;
    UDP &operator=(const UDP &other) = delete;
    UDP &operator=(UDP &&other) = delete;

private:
    void listen(UDPCallback callback);

    int _socket;
    std::jthread _listen_thread;
    bool _listen_thread_running = false;
    Fix _listen_buffer;
};

} // namespace zouipocar

#endif
