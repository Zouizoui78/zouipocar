#include "gtest/gtest.h"

#include "../../common/common_constants.h"
#include "UDP.hpp"

#include <arpa/inet.h>

using namespace zouipocar;

TEST(TestUDP, test_receiver_callback) {
    bool callback_called = false;
    size_t received_bytes = 0;
    UDP udp(5000, [&](const std::vector<uint8_t>& data) {
        callback_called = true;
        received_bytes = data.size();
        const char *str = reinterpret_cast<const char*>(data.data());
    });

    std::string data("azertyuiopqs");

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in addr_in;
    addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_in.sin_port = htons(5000);
    addr_in.sin_family = AF_INET;
    sockaddr *addr = reinterpret_cast<sockaddr *>(&addr_in);

    int res = sendto(s, data.data(), data.size() + 1, 0, addr, sizeof(addr_in));

    usleep(1e3);

    ASSERT_NE(res, -1);
    ASSERT_TRUE(callback_called);
    ASSERT_EQ(received_bytes, ZOUIPOCAR_PACKET_SIZE);
}