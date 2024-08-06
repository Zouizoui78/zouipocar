#include "gtest/gtest.h"
#include "HTTPServer.hpp"

class TestHTTPServer : public ::testing::Test {
    protected:

    TestHTTPServer() :
        server(path)
    {}

    void SetUp() override {
        server_thread = std::thread([this](){
            server.listen("0.0.0.0", port);
        });

        client = new httplib::Client("localhost", port);
    }

    void TearDown() override {
        server.stop();
        server_thread.join();

        delete client;
    }

    std::string path = "./test/test_resources/test.db";
    HTTPServer server;
    std::thread server_thread;

    httplib::Client *client;

    int port = 3001;
};

TEST_F(TestHTTPServer, test_static_files) {
    auto res = client->Get("/");
    ASSERT_TRUE(res);

    std::ifstream index("www/index.html");
    std::stringstream index_str;

    index_str << index.rdbuf();

    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->body, index_str.str());

    res = client->Get("/test");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);
}

TEST_F(TestHTTPServer, test_fix) {
    Database db(path);
    ASSERT_TRUE(db.is_open());

    auto res = client->Get("/api/fix?date=first");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body), db.query_first_fix());

    res = client->Get("/api/fix?date=last");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body), db.query_last_fix());

    json record = {
        { "timestamp", 1646722281 },
        { "speed", 11 },
        { "latitude", 48.76424 },
        { "longitude", 2.036607 }
    };
    res = client->Get("/api/fix?date=1646722281");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body), record);

    res = client->Get("/api/fix?date=nonesense");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
}

TEST_F(TestHTTPServer, test_range) {
    Database db(path);
    ASSERT_TRUE(db.is_open());

    auto res = client->Get("/api/range?start=1646722277&stop=1646722282");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body).size(), 6);

    res = client->Get("/api/range?start=1646722277&stop=1646722277");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);

    res = client->Get("/api/range?start=1646722282&stop=1646722277");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);

    res = client->Get("/api/range?start=gibberish&stop=random");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
}