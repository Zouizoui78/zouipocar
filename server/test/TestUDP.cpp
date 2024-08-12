#include "gtest/gtest.h"
#include "test_tools.hpp"

#include "Fix.hpp"
#include "UDP.hpp"

#include <arpa/inet.h>
#include <cstdint>
#include <vector>

using namespace zouipocar;
using namespace zouipocar_test;

TEST(TestUDP, test_receiver_callback) {
    bool callback_called = false;
    fix::Fix received;
    UDP udp(5000, [&](const fix::Fix& fix) {
        callback_called = true;
        received = fix;
    });

    fix::Fix fix;
    fix.timestamp = 1723276072;
    fix.latitude = 48.931835;
    fix.longitude = 2.054971;
    fix.speed = 123;

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in addr_in;
    addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_in.sin_port = htons(5000);
    addr_in.sin_family = AF_INET;
    sockaddr *addr = reinterpret_cast<sockaddr *>(&addr_in);

    int res = sendto(s, &fix, sizeof fix, 0, addr, sizeof (addr_in));

    usleep(1e3);

    ASSERT_NE(res, -1);
    ASSERT_TRUE(callback_called);
    compare_fixes(fix, received);
}