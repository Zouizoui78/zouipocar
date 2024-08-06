#include "gtest/gtest.h"
#include "Database.hpp"

TEST(TestDatabase, test_file_creation) {
    std::string path = std::string(std::getenv("TEST_OUTPUTS")) + "/test.db";
    Database db(path);

    ASSERT_TRUE(db.is_open());
    ASSERT_TRUE(std::filesystem::exists(path));

    std::filesystem::remove(path);
}

TEST(TestDatabase, test_insert) {
    json j = {
        { "timestamp", 1649577294 },
        { "speed", 123 },
        { "latitude", 15 },
        { "longitude", 30 }
    };

    std::string path = std::string(std::getenv("TEST_OUTPUTS")) + "/test.db";
    Database db(path);
    ASSERT_TRUE(db.is_open());
    ASSERT_TRUE(db.insert_fix(j));
    ASSERT_EQ(j, db.query_fix(1649577294));

    std::filesystem::remove(path);
}

TEST(TestDatabase, test_query) {
    Database db("./test/test_resources/test.db");
    ASSERT_TRUE(db.is_open());

    json first_record = {
        { "timestamp", 1646722255 },
        { "speed", 5 },
        { "latitude", 48.76503 },
        { "longitude", 2.037482 }
    };

    json last_record = {
        { "timestamp", 1646722336 },
        { "speed", 33 },
        { "latitude", 48.763393 },
        { "longitude", 2.036618 }
    };

    json record = {
        { "timestamp", 1646722281 },
        { "speed", 11 },
        { "latitude", 48.76424 },
        { "longitude", 2.036607 }
    };

    ASSERT_EQ(first_record, db.query_first_fix());
    ASSERT_EQ(last_record, db.query_last_fix());
    ASSERT_EQ(record, db.query_fix(1646722281));

    json range = db.query_fix_range(1646722264, 1646722270);
    ASSERT_EQ(range.size(), 7);

    ASSERT_TRUE(db.query_fix(1).is_null());
    ASSERT_TRUE(db.query_fix_range(1, 50).is_null());
}