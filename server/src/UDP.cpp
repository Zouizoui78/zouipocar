#include "UDP.hpp"

#include <iostream>
#include <string.h> // For strerror()
#include <unistd.h> // For close()
#include <arpa/inet.h>

UDP::UDP(uint16_t port) {
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket == -1) {
        std::cout << "Failed to create UDP socket.\n";
        _ok = false;
    }

    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(port);
    _addr.sin_family = AF_INET;

    if (bind(_socket, reinterpret_cast<sockaddr *>(&_addr), sizeof(_addr)) != 0) {
        std::cout << "Failed to bind UDP socket.\n";
        _ok = false;
    }

    _listen_buffer = reinterpret_cast<uint8_t *>(calloc(UDP_MAX_PACKET_SIZE, sizeof(uint8_t)));

    _ok = true;
    std::cout << "Opened UDP socket on port " << port << std::endl;
}
    
UDP::~UDP() {
    if (is_ok()) {
        shutdown(_socket, SHUT_RDWR); // Interrupt blocking recvfrom.
        ::close(_socket);

        if (_listen_thread_running) {
            _listen_thread_running = false;
            _listen_thread.join();
        }

        _ok = false;
        std::cout << "Closed UDP socket on port " << ntohs(_addr.sin_port) << std::endl;
    }

    if (_listen_buffer != nullptr) {
        free(_listen_buffer);
    }
}

bool UDP::is_ok() {
    return _ok;
}

bool UDP::start_listen(std::function<void (uint8_t *data, size_t size)> callback) {
    if (!callback) {
        std::cout << "Cannot start listening, invalid callback.\n";
        return false;
    }

    if (_listen_thread_running) {
        std::cout << "Cannot start listening, already running.\n";
        return false;
    }

    _listen_callback = callback;
    _listen_thread = std::thread([this]() {
        sockaddr_in from;
        socklen_t fromlen = sizeof(from);

        _listen_thread_running = true;
        while (_listen_thread_running) {
            ssize_t size = recvfrom(_socket, _listen_buffer, UDP_MAX_PACKET_SIZE, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
            if (size == -1) {
                std::cout << "Failed to receive data : " << strerror(errno) << std::endl;
            }
            else if (size > 0) {
                _listen_callback(_listen_buffer, size);
            }
        }
    });

    return true;
}