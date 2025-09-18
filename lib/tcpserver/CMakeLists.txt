cmake_minimum_required(VERSION 3.10)
project(tcpserver)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Platform-specific sources
if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "BSD")
    set(PLATFORM_SOURCES src/kqueue_loop.cpp)
    add_definitions(-DUSE_KQUEUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(PLATFORM_SOURCES src/epoll_loop.cpp)
    add_definitions(-DUSE_EPOLL)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Create the library
add_library(tcpserver STATIC
    src/connection.cpp
    src/tcp_server.cpp
    src/event_loop_factory.cpp
    ${PLATFORM_SOURCES}
)

# Set the output directory
set_target_properties(tcpserver PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Install the library
install(TARGETS tcpserver
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY include/tcpserver/
    DESTINATION include/tcpserver
    FILES_MATCHING PATTERN "*.hpp"
)
