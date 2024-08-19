#include "gtest/gtest.h"

#include "Database.hpp"
#include "HTTPServer.hpp"
#include "constants.hpp"
#include "test_tools.hpp"

using namespace zouipocar;
using namespace zouipocar_test;

Fix deserialize_fix(const std::string &f) {
    Fix fix;
    memcpy(&fix, f.data(), sizeof(Fix));
    return fix;
}

std::vector<Fix> deserialize_fixes(const std::string &f) {
    std::vector<Fix> fixes;
    fixes.resize(f.size() / sizeof(Fix));
    memcpy(fixes.data(), f.data(), f.size());
    return fixes;
}

class TestHTTPServer : public ::testing::Test {
protected:
    TestHTTPServer()
        : db(path), server("www", &db), client("localhost", ZOUIPOCAR_PORT) {}

    void SetUp() override {
        server_thread = std::jthread([this]() {
            server.listen("0.0.0.0", ZOUIPOCAR_PORT);
        });
        server.wait_until_ready();
    }

    void TearDown() override {
        server.stop();
    }

    std::string path = "./test/test_resources/test.db";
    Database db;
    HTTPServer server;
    std::jthread server_thread;
    httplib::Client client;
};

TEST_F(TestHTTPServer, test_static_files) {
    auto res = client.Get("/");
    ASSERT_TRUE(res);

    std::ifstream index("www/index.html");
    std::stringstream index_str;

    index_str << index.rdbuf();

    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->body, index_str.str());

    res = client.Get("/test");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);
}

TEST_F(TestHTTPServer, test_fix) {
    auto res = client.Get("/api/fix?date=1646722281");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);

    res = client.Get("/api/fix?date=123");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);

    res = client.Get("/api/fix?date=nonesense");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
}

TEST_F(TestHTTPServer, test_first_fix) {
    auto res = client.Get("/api/fix/first");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    compare_fixes(deserialize_fix(res->body), db.get_first_fix().value());
}

TEST_F(TestHTTPServer, test_last_fix) {
    auto res = client.Get("/api/fix/last");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    compare_fixes(deserialize_fix(res->body), db.get_last_fix().value());
}

TEST_F(TestHTTPServer, test_range) {
    auto res = client.Get("/api/range?start=1646722277&stop=1646722282");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(deserialize_fixes(res->body).size(), 6);

    res = client.Get("/api/range?start=1646722277&stop=1646722277");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);

    res = client.Get("/api/range?start=1646722282&stop=1646722277");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);

    res = client.Get("/api/range?start=gibberish&stop=random");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
}