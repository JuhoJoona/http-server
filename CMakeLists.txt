cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_CXX_STANDARD 17)


include_directories(src/client src/server)

add_executable(http-server
    src/main.cpp
)
