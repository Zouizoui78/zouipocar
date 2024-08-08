#include "gtest/gtest.h"

#include "constants.hpp"
#include "Database.hpp"
#include "HTTPServer.hpp"
#include "json.hpp"
#include "test_tools.hpp"

using json = nlohmann::json;

namespace zouipocar {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Fix, timestamp, speed, latitude, longitude);
}

using namespace zouipocar;

class TestHTTPServer : public ::testing::Test {
    protected:

    TestHTTPServer()
        : db(path), server("www", &db), client("localhost", PORT)
    {}

    void SetUp() override {
        server_thread = std::jthread([this](){
            server.listen("0.0.0.0", PORT);
        });
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
    auto res = client.Get("/api/fix/first");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body), db.get_first_fix());

    res = client.Get("/api/fix/last");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body), db.get_last_fix());

    res = client.Get("/api/fix?date=1646722281");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);

    res = client.Get("/api/fix?date=123");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);

    res = client.Get("/api/fix?date=nonesense");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
}

TEST_F(TestHTTPServer, test_range) {
    auto res = client.Get("/api/range?start=1646722277&stop=1646722282");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body).size(), 6);

    res = client.Get("/api/range?start=123&stop=1234");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);

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

TEST_F(TestHTTPServer, test_poll_fix) {
    httplib::Result res;

    int n_threads = 3;
    std::thread thread([this, &res]{
        res = client.Get("/api/pollfix");
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    Fix f;
    f.timestamp = 1649577294;
    f.speed = 123;
    f.latitude = 15;
    f.longitude =  30;

    server.update_last_fix(f);
    thread.join();

    ASSERT_TRUE(res);
    Fix received = json::parse(res->body);
    zouipocar_test::compare_fixes(f, received);
}