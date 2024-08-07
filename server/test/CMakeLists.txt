find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

set(TEST_SRC
    test_tools.cpp
    test_tools.hpp
    TestDatabase.cpp
    TestFix.cpp
    TestHTTPServer.cpp
    TestTools.cpp
    TestUDP.cpp
)

add_executable(zouipocar-test ${TEST_SRC})

target_include_directories(zouipocar-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(zouipocar-test
    PRIVATE GTest::gtest_main
)
target_link_libraries(zouipocar-test PRIVATE zouipocar)

gtest_discover_tests(zouipocar-test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    PROPERTIES
        LABELS test
        ENVIRONMENT "TEST_OUTPUT_DIR=build/test_output"
)
