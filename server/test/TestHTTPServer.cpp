#include "gtest/gtest.h"

#include "Database.hpp"
#include "http/ErrorMessages.hpp"
#include "http/HTTPServer.hpp"
#include "test_tools.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <thread>
#include <vector>

using namespace zouipocar;
using namespace zouipocar::http;
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
        : db(path), server("www", &db), client("localhost", port),
          empty_db(empty_db_path), empty_db_server("www", &empty_db),
          empty_db_client("localhost", empty_db_port) {}

    void SetUp() override {
        server_thread = std::jthread([this]() {
            server.listen("0.0.0.0", port);
        });

        empty_db_server_thread = std::jthread([this]() {
            empty_db_server.listen("0.0.0.0", empty_db_port);
        });

        server.wait_until_ready();
        empty_db_server.wait_until_ready();
    }

    void TearDown() override {
        server.stop();
        empty_db_server.stop();
    }

    std::string path = "./test/test_resources/test.db";
    Database db;
    HTTPServer server;
    int port = 9999;
    std::jthread server_thread;
    httplib::Client client;

    std::string empty_db_path =
        (std::filesystem::path(std::getenv("TEST_OUTPUT_DIR")) / "empty.db")
            .string();
    Database empty_db;
    HTTPServer empty_db_server;
    int empty_db_port = 9998;
    std::jthread empty_db_server_thread;
    httplib::Client empty_db_client;
};

TEST_F(TestHTTPServer, test_static_files) {
    auto res = client.Get("/");
    ASSERT_TRUE(res);

    std::ifstream index("www/index.html");
    std::stringstream index_str;

    index_str << index.rdbuf();

    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->body, index_str.str());
}

TEST_F(TestHTTPServer, test_static_file_not_found) {
    auto res = client.Get("/non-existing-file");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);
}

TEST_F(TestHTTPServer, test_fix) {
    auto res = client.Get("/api/fix?date=1646722281");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
}

TEST_F(TestHTTPServer, test_fix_not_found) {
    auto res = client.Get("/api/fix?date=123");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);
}

TEST_F(TestHTTPServer, test_fix_invalid_date_parameter) {
    auto res = client.Get("/api/fix?date=nonesense");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
    EXPECT_EQ(ErrorMessages::api_fix_invalid_parameter, res->body);
}

TEST_F(TestHTTPServer, test_fix_missing_date_parameter) {
    auto res = client.Get("/api/fix");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
    EXPECT_EQ(ErrorMessages::api_fix_missing_parameter, res->body);
}

TEST_F(TestHTTPServer, test_fix_out_of_range_date_parameter) {
    auto res = client.Get("/api/fix?date=111111111111111111111");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 400);
    EXPECT_EQ(ErrorMessages::api_fix_out_of_range_parameter, res->body);
}

TEST_F(TestHTTPServer, test_first_fix) {
    auto res = client.Get("/api/fix/first");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    expect_fix_eq(deserialize_fix(res->body), db.get_first_fix().value());
}

TEST_F(TestHTTPServer, test_first_fix_not_found) {
    auto res = empty_db_client.Get("/api/fix/first");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);
}

TEST_F(TestHTTPServer, test_last_fix) {
    auto res = client.Get("/api/fix/last");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    expect_fix_eq(deserialize_fix(res->body), db.get_last_fix().value());
}

TEST_F(TestHTTPServer, test_last_fix_not_found) {
    auto res = empty_db_client.Get("/api/fix/last");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 404);
}

TEST_F(TestHTTPServer, test_range) {
    auto res = client.Get("/api/range?start=1646722277&stop=1646722282");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(deserialize_fixes(res->body).size(), 6);
}

TEST_F(TestHTTPServer, test_range_missing_parameter) {
    auto check_res = [](const httplib::Result &res) {
        ASSERT_TRUE(res);
        EXPECT_EQ(res->status, 400);
        EXPECT_EQ(res->body, ErrorMessages::api_range_missing_parameter);
    };

    check_res(client.Get("/api/range?start=gibberish"));
    check_res(client.Get("/api/range?stop=gibberish"));
    check_res(client.Get("/api/range"));
}

TEST_F(TestHTTPServer, test_range_invalid_parameter) {
    auto check_res = [](const httplib::Result &res) {
        ASSERT_TRUE(res);
        EXPECT_EQ(res->status, 400);
        EXPECT_EQ(res->body, ErrorMessages::api_range_invalid_parameter);
    };

    check_res(client.Get("/api/range?start=gibberish&stop=random"));
    check_res(client.Get("/api/range?start=123&stop=random"));
    check_res(client.Get("/api/range?start=gibberish&stop=123"));
}

TEST_F(TestHTTPServer, test_range_out_of_range_parameter) {
    auto check_res = [](const httplib::Result &res) {
        ASSERT_TRUE(res);
        EXPECT_EQ(res->status, 400);
        EXPECT_EQ(res->body, ErrorMessages::api_range_out_of_range_parameter);
    };

    check_res(
        client.Get("/api/range?start=111111111111111111111&stop=1646722277"));
    check_res(
        client.Get("/api/range?start=1646722277&stop=111111111111111111111"));
    check_res(client.Get(
        "/api/range?start=111111111111111111111&stop=111111111111111111111"));
}

TEST_F(TestHTTPServer, test_range_invalid_range) {
    auto check_res = [](const httplib::Result &res) {
        ASSERT_TRUE(res);
        EXPECT_EQ(res->status, 400);
        EXPECT_EQ(res->body, ErrorMessages::api_range_invalid_range);
    };

    check_res(client.Get("/api/range?start=1646722277&stop=1646722277"));
    check_res(client.Get("/api/range?start=1646722278&stop=1646722277"));
}

TEST_F(TestHTTPServer, test_update_fix) {
    Fix f{1, 2, 3, 4};
    server.update_fix(f);
    auto res = client.Get("/api/fix/last");
    expect_fix_eq(f, deserialize_fix(res->body));
}