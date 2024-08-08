#include "gtest/gtest.h"
#include "Database.hpp"
#include "test_tools.hpp"

#include <filesystem>

using namespace zouipocar;
using namespace zouipocar_test;

TEST(TestDatabase, test_insert) {
    std::filesystem::path output_dir(std::getenv("TEST_OUTPUT_DIR"));
    std::filesystem::path path(output_dir / "test.db");
    std::filesystem::create_directory(output_dir);
    Database* db = new Database(path.string());

    Fix f;
    f.timestamp = 1649577294;
    f.speed = 123;
    f.latitude = 15;
    f.longitude =  30;

    bool inserted = db->insert_fix(f);
    auto queried = db->get_fix(1649577294);

    ASSERT_TRUE(inserted);
    compare_fixes(f, *queried);

    delete db;
    std::filesystem::remove(path);
}

TEST(TestDatabase, test_get_fix) {
    Database db("./test/test_resources/test.db");

    Fix first_record;
    first_record.timestamp = 1646722255;
    first_record.speed = 5;
    first_record.latitude = 48.76503;
    first_record.longitude = 2.03748;

    Fix last_record;
    last_record.timestamp = 1646722336;
    last_record.speed = 33;
    last_record.latitude = 48.763393;
    last_record.longitude = 2.03661;

    Fix record;
    record.timestamp = 1646722281;
    record.speed = 11;
    record.latitude = 48.76424;
    record.longitude = 2.03660;

    compare_fixes(record, db.get_fix(1646722281).value());
    ASSERT_FALSE(db.get_fix(1).has_value());
}

TEST(TestDatabase, test_get_first_fix) {
    Database db("./test/test_resources/test.db");

    Fix fix;
    fix.timestamp = 1646722255;
    fix.speed = 5;
    fix.latitude = 48.76503;
    fix.longitude = 2.03748;

    compare_fixes(fix, db.get_first_fix().value());
}

TEST(TestDatabase, test_get_last_fix) {
    Database db("./test/test_resources/test.db");

    Fix fix;
    fix.timestamp = 1646722336;
    fix.speed = 33;
    fix.latitude = 48.763393;
    fix.longitude = 2.03661;

    compare_fixes(fix, db.get_last_fix().value());
}

TEST(TestDatabase, test_get_fix_range) {
    Database db("./test/test_resources/test.db");
    std::vector<Fix> range = db.get_fix_range(1646722264, 1646722270);
    ASSERT_EQ(range.size(), 7);
    ASSERT_EQ(db.get_fix_range(1, 50).size(), 0);
}
