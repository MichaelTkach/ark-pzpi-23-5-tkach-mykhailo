cmake_minimum_required(VERSION 3.14)
project(EcoRecycleServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/database.cpp
    src/controllers.cpp
)

add_executable(server ${SOURCES})

target_link_libraries(server PRIVATE SQLite3::SQLite3 pthread)
