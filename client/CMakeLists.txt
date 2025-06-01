add_executable(perfmon_client
    src/main.cpp
    src/client.cpp
)

target_include_directories(perfmon_client PRIVATE include)
target_link_libraries(perfmon_client PRIVATE common)

if(WIN32)
    target_link_libraries(perfmon_client PRIVATE wsock32 ws2_32)
endif() 