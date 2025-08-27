cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Add all source files
add_executable(http-server
    src/main.cpp
    src/net/server.cpp
    src/net/connection.cpp
    src/http/router.cpp
    src/http/parser.cpp
)
