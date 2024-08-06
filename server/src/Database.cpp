#include "Database.hpp"

#include <ctime>

Database::Database(const std::string &path) {
    _path = path;

    if (_path.empty()) {
        std::cout << "Cannot open database, empty path.\n";
        return;
    }

    int res = sqlite3_open(_path.c_str(), &_db_handler);
    if (res != SQLITE_OK || _db_handler == nullptr) {
        std::cout << "Failed to open database file " << _path << " : " << sqlite3_errmsg(_db_handler) << std::endl;
        return;
    }

    res = sqlite3_exec(_db_handler, "PRAGMA synchronous = OFF", nullptr, nullptr, &_errmsg);
    if (res != SQLITE_OK) {
        std::cout << "Failed to disable fs sync for database.\n";
    }

    _is_open = create_table();

    if (_is_open)
        std::cout << "Opened database file '" << _path << "'" << std::endl;
    else
        std::cout << "Failed to create table zoui in database.\n";
}

Database::~Database() {
    if (is_open()) {
        int res = sqlite3_close(_db_handler);
        if (res != SQLITE_OK) {
            std::cout << "Failed to close database handler for " << _path << std::endl;
        }
    }

    _is_open = false;
    std::cout << "Closed database file '" << _path << "'" << std::endl;
}

bool Database::is_open() {
    return _is_open && _db_handler != nullptr;
}

bool Database::create_table() {
    std::string statement = "create table if not exists zoui (timestamp INTEGER, speed INTEGER, latitude REAL, longitude REAL);";
    int res = sqlite3_exec(_db_handler, statement.c_str(), nullptr, nullptr, &_errmsg);
    if (res != SQLITE_OK) {
        std::cout << "Failed to create table zoui : " << _errmsg << std::endl;
        return false;
    }
    return true;
}

bool Database::insert_fix(json fix) {
    std::string statement = "insert into zoui values(";
    statement += std::to_string(fix["timestamp"].get<time_t>()) + ",";
    statement += std::to_string(fix["speed"].get<uint8_t>()) + ",";
    statement += std::to_string(fix["latitude"].get<float>()) + ",";
    statement += std::to_string(fix["longitude"].get<float>()) + ");";
    int res = sqlite3_exec(_db_handler, statement.c_str(), nullptr, nullptr, &_errmsg);

    if (res != SQLITE_OK) {
        std::cout << "Failed to insert fix into zoui : " << _errmsg << std::endl;
        return false;
    }

    return true;
}

json Database::query_fix(time_t date) {
    json j;

    bool ok = query("SELECT * FROM zoui WHERE timestamp=" + std::to_string(date), [this, &j](sqlite3_stmt *stmt) {
        j = initialize_fix_json();
        j["timestamp"] = sqlite3_column_int64(stmt, 0);
        j["speed"] = sqlite3_column_int64(stmt, 1);
        j["latitude"] = sqlite3_column_double(stmt, 2);
        j["longitude"] = sqlite3_column_double(stmt, 3);
    });

    if (!ok) {
        std::cout << "Failed to query fix with timestamp = " << date << std::endl;
    }

    return j;
}

json Database::query_fix_range(time_t start, time_t end) {
    json j;

    bool ok = query("SELECT * FROM zoui WHERE timestamp BETWEEN " + std::to_string(start) + " AND " + std::to_string(end), [this, &j](sqlite3_stmt *stmt) {
        json to_add = {
            { "timestamp", sqlite3_column_int64(stmt, 0) },
            { "speed", sqlite3_column_int64(stmt, 1) },
            { "latitude", sqlite3_column_double(stmt, 2) },
            { "longitude", sqlite3_column_double(stmt, 3) }
        };
        j.push_back(to_add);
    });

    if (!ok) {
        std::cout << "Failed to query fixes between = " << start << " and " << end << std::endl;
    }

    return j;
}

json Database::query_first_fix() {
    json j = initialize_fix_json();
    bool ok = query("SELECT * FROM zoui LIMIT 1;", [this, &j](sqlite3_stmt *stmt) {
        j["timestamp"] = sqlite3_column_int64(stmt, 0);
        j["speed"] = sqlite3_column_int64(stmt, 1);
        j["latitude"] = sqlite3_column_double(stmt, 2);
        j["longitude"] = sqlite3_column_double(stmt, 3);
    });

    if (!ok) {
        std::cout << "Failed to query last fix\n";
    }

    return j;
}

json Database::query_last_fix() {
    json j = initialize_fix_json();
    bool ok = query("SELECT * FROM zoui ORDER BY timestamp DESC LIMIT 1;", [this, &j](sqlite3_stmt *stmt) {
        j["timestamp"] = sqlite3_column_int64(stmt, 0);
        j["speed"] = sqlite3_column_int64(stmt, 1);
        j["latitude"] = sqlite3_column_double(stmt, 2);
        j["longitude"] = sqlite3_column_double(stmt, 3);
    });

    if (!ok) {
        std::cout << "Failed to query last fix\n";
    }

    return j;
}

json Database::initialize_fix_json() {
    json j = {
        { "timestamp", 0 },
        { "speed", 0 },
        { "latitude", 0 },
        { "longitude", 0 }
    };
    return j;
}