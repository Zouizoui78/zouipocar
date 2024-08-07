#include "constants.hpp"
#include "UDP.hpp"

#include <iostream>
#include <string.h> // For strerror()
#include <unistd.h> // For close()
#include <arpa/inet.h>

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

    _listen_buffer.resize(PACKET_SIZE);
}

UDP::~UDP() {
    shutdown(_socket, SHUT_RDWR); // Interrupt blocking recvfrom.
    ::close(_socket);
    _listen_thread_running = false;
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
            ssize_t size = recvfrom(_socket, _listen_buffer.data(), PACKET_SIZE, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
            if (size == -1) {
                std::cout << "UDP : Failed to receive data : " << strerror(errno) << std::endl;
            }
            else if (size != PACKET_SIZE) {
                std::cout << std::format("Received invalid packet, size = {} instead of {}", size, PACKET_SIZE);
            }
            else {
                callback(_listen_buffer);
            }
        }
    });
}

}
