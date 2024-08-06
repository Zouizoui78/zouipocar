find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

include(${PROJECT_SOURCE_DIR}/cmake/src.cmake)
set(TEST_SRC
    TestDatabase.cpp
    TestHTTPServer.cpp
    TestTools.cpp
    TestUDP.cpp
)

add_executable(zouipocar-test ${SRC} ${TEST_SRC})

target_include_directories(zouipocar-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(zouipocar-test
    PRIVATE GTest::gtest_main
)
target_link_libraries(zouipocar-test PRIVATE SQLite::SQLite3)

gtest_discover_tests(zouipocar-test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    PROPERTIES
        LABELS test
        ENVIRONMENT "TEST_OUTPUT_DIR=build/test_output"
)