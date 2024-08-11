#include "../../common/common_constants.h"
#include "UDP.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <format>
#include <iostream>

namespace zouipocar {

UDP::UDP(uint16_t port, UDPCallback callback) {
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket == -1) {
        throw std::runtime_error("Failed to create UDP socket");
    }

    sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    if (bind(_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) {
        throw std::runtime_error("Failed to bind UDP socket");
    }

    _listen_buffer.resize(ZOUIPOCAR_PACKET_SIZE);
    listen(callback);
}

UDP::~UDP() {
    _listen_thread_running = false;
    shutdown(_socket, SHUT_RDWR); // Interrupt blocking recvfrom.
    ::close(_socket);
}

void UDP::listen(UDPCallback callback) {
    if (!callback) {
        throw std::runtime_error("Cannot start listening, invalid callback");
    }

    _listen_thread = std::jthread([this, callback]() {
        sockaddr_in from;
        socklen_t fromlen = sizeof(from);

        _listen_thread_running = true;
        while (_listen_thread_running) {
            ssize_t size = recvfrom(
                _socket,
                _listen_buffer.data(),
                ZOUIPOCAR_PACKET_SIZE,
                0,
                reinterpret_cast<sockaddr*>(&from),
                &fromlen
            );
            if (size == -1) {
                std::cout << "UDP : Failed to receive data : " << strerror(errno) << std::endl;
            }
            else if (size != ZOUIPOCAR_PACKET_SIZE && _listen_thread_running == true) {
                std::cout << std::format("Received invalid packet, size = {} instead of {}", size, ZOUIPOCAR_PACKET_SIZE) << std::endl;
            }
            else if (_listen_thread_running) {
                callback(_listen_buffer);
            }
        }
    });
}

}
