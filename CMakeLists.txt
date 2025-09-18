cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)
include_directories(lib/tcpserver/include)

# Build the TCP server library
add_subdirectory(lib/tcpserver)

#platform specific libraries for HTTP server
if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "BSD")
    set(PLATFORM_SOURCES src/eventloop/kqueue_loop.cpp)
    add_definitions(-DUSE_KQUEUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(PLATFORM_SOURCES src/eventloop/epoll_loop.cpp)
    add_definitions(-DUSE_EPOLL)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Add all source files for HTTP server
add_executable(http-server
    src/main.cpp
    src/net/server.cpp
    src/net/connection.cpp
    src/http/router.cpp
    src/http/parser.cpp
    src/http/http_connection_handler.cpp
    src/eventloop/event_loop_factory.cpp
    ${PLATFORM_SOURCES}
)

# Link the TCP server library
target_link_libraries(http-server tcpserver)
