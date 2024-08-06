#include "gtest/gtest.h"
#include "UDP.hpp"

#include <arpa/inet.h>

TEST(TestUDP, test_receiver_callback) {
    UDP udp(5000);
    ASSERT_TRUE(udp.is_ok());

    bool callback_called = false;
    size_t received_bytes = 0;
    udp.start_listen([&](uint8_t *data, size_t size) {
        callback_called = true;
        received_bytes = size;

        char *str = (char *)data;
        printf("Received %ld bytes : %s\n", size, str);
    });

    char data[10];
    strcpy(data, "test_data");

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in addr_in;
    addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_in.sin_port = htons(5000);
    addr_in.sin_family = AF_INET;
    sockaddr *addr = reinterpret_cast<sockaddr *>(&addr_in);

    int res = sendto(s, data, strlen(data) + 1, 0, addr, sizeof(addr_in));

    usleep(1e3);

    ASSERT_NE(res, -1);
    ASSERT_TRUE(callback_called);
    ASSERT_EQ(received_bytes, 10);
}