#include "test_tools.hpp"
#include "gtest/gtest.h"

#include "Fix.hpp"
#include "UDP.hpp"

#include <arpa/inet.h>
#include <cstdint>
#include <vector>

using namespace zouipocar;
using namespace zouipocar_test;

constexpr int port = 5000;

int send_to_listener(const void *buffer, size_t size) {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in addr_in;
    addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_in.sin_port = htons(port);
    addr_in.sin_family = AF_INET;
    sockaddr *addr = reinterpret_cast<sockaddr *>(&addr_in);

    return sendto(s, buffer, size, 0, addr, sizeof(addr_in));
}

TEST(TestUDP, test_bind_fail) {
    std::unique_ptr<UDP> first_listener;
    ASSERT_NO_THROW(first_listener =
                        std::make_unique<UDP>(port, [](const Fix &fix) {}));
    ASSERT_THROW(UDP(port, [](const Fix &fix) {}), std::runtime_error);
}

TEST(TestUDP, test_invalid_callback) {
    ASSERT_THROW(UDP(port, UDP::UDPCallback()), std::runtime_error);
}

TEST(TestUDP, test_wrong_packet_size) {
    bool callback_called = false;
    UDP udp(port, [&callback_called](const Fix &fix) {
        callback_called = true;
    });

    constexpr int size = 3;
    uint8_t buffer[size];
    int res = send_to_listener(buffer, size);
    usleep(1e3);

    ASSERT_NE(res, -1);
    ASSERT_FALSE(callback_called);
}

TEST(TestUDP, test_receiver_callback) {
    bool callback_called = false;
    Fix received;
    UDP udp(port, [&callback_called, &received](const Fix &fix) {
        callback_called = true;
        received = fix;
    });

    Fix fix;
    fix.timestamp = 1723276072;
    fix.latitude = 48.931835;
    fix.longitude = 2.054971;
    fix.speed = 123;

    int res = send_to_listener(&fix, sizeof fix);
    usleep(1e3);

    ASSERT_NE(res, -1);
    ASSERT_TRUE(callback_called);
    expect_fix_eq(fix, received);
}