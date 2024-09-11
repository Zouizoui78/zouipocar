cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    zouipocar-server
    VERSION 0.0.1
    LANGUAGES CXX
)

option(ZOUIPOCAR_TESTS "Build unit tests" ON)

set(
    SRC
    ${PROJECT_SOURCE_DIR}/src/Database.cpp
    ${PROJECT_SOURCE_DIR}/src/Database.hpp
    ${PROJECT_SOURCE_DIR}/src/Fix.hpp
    ${PROJECT_SOURCE_DIR}/src/http/ErrorMessages.cpp
    ${PROJECT_SOURCE_DIR}/src/http/ErrorMessages.hpp
    ${PROJECT_SOURCE_DIR}/src/http/HTTPServer.cpp
    ${PROJECT_SOURCE_DIR}/src/http/HTTPServer.hpp
    ${PROJECT_SOURCE_DIR}/src/UDP.cpp
    ${PROJECT_SOURCE_DIR}/src/UDP.hpp
    ${PROJECT_SOURCE_DIR}/external/httplib.cpp
    ${PROJECT_SOURCE_DIR}/external/httplib.h
)

add_library(zouipocar STATIC ${SRC})

target_include_directories(zouipocar PUBLIC src)
target_include_directories(zouipocar PUBLIC external)

find_package(SQLite3 REQUIRED)
target_link_libraries(zouipocar PRIVATE SQLite::SQLite3)

add_executable(zouipocar-exe src/main.cpp)

target_link_libraries(zouipocar-exe PRIVATE zouipocar)

set_property(TARGET zouipocar-exe PROPERTY OUTPUT_NAME zouipocar)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if (ZOUIPOCAR_TESTS)
        include(CTest)
        add_subdirectory(test)
        target_compile_options(zouipocar PRIVATE --coverage -g -O0)
        target_link_libraries(zouipocar PRIVATE gcov)
    endif (ZOUIPOCAR_TESTS)
endif()
