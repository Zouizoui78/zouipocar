cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    zouipocar-server
    VERSION 0.0.1
    LANGUAGES CXX
)

include(cmake/src.cmake)

option(ZOUIPOCAR_TESTS "Build unit tests" ON)

add_library(zouipocar STATIC ${SRC})

target_include_directories(zouipocar PUBLIC external)

find_package(SQLite3 REQUIRED)
target_link_libraries(zouipocar PRIVATE SQLite::SQLite3)

add_executable(zouipocar_exe src/main.cpp)
target_link_libraries(zouipocar_exe zouipocar)
set_property(TARGET zouipocar_exe PROPERTY OUTPUT_NAME zouipocar)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if (ZOUIPOCAR_TESTS)
        include(CTest)
        add_subdirectory(test)
    endif (ZOUIPOCAR_TESTS)
endif()