cmake_minimum_required(VERSION 3.10)
project(PerformanceMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for server and client
add_subdirectory(server)
add_subdirectory(client)

# Add common library
add_library(common
    include/common/metrics.hpp
    include/common/socket_wrapper.hpp
    src/common/metrics.cpp
    src/common/socket_wrapper.cpp
)

target_include_directories(common PUBLIC include)

# Link WinSock and PDH libraries on Windows
if(WIN32)
    target_link_libraries(common PRIVATE wsock32 ws2_32 pdh)
endif() 