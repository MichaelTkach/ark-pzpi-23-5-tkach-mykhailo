cmake_minimum_required(VERSION 3.14)
project(EcoRecycleServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/database.cpp
    src/controllers.cpp
)

# Executable
add_executable(server ${SOURCES})

# Link libraries
# Assuming Crow is header-only and installed or available in include path. 
# For this lab, we assume system-wide install or simple availability. 
# If not, one would usually use FetchContent.
# Linking SQLite3 and pthread (often needed for Crow/asio)
target_link_libraries(server PRIVATE SQLite3::SQLite3 pthread)
