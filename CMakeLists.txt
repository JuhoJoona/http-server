cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

#platform specific libraries
if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "BSD")
    set(PLATFORM_SOURCES src/eventloop/kqueue_loop.cpp)
    add_definitions(-DUSE_KQUEUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(PLATFORM_SOURCES src/eventloop/epoll_loop.cpp)
    add_definitions(-DUSE_EPOLL)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Add all source files
add_executable(http-server
    src/main.cpp
    src/net/server.cpp
    src/net/connection.cpp
    src/http/router.cpp
    src/http/parser.cpp
    src/eventloop/event_loop_factory.cpp
    ${PLATFORM_SOURCES}
)
