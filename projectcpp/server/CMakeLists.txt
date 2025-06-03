add_executable(perfmon_server
    src/main.cpp
    src/server.cpp
    src/data_store.cpp
)

target_include_directories(perfmon_server PRIVATE include)
target_link_libraries(perfmon_server PRIVATE common)

if(WIN32)
    target_link_libraries(perfmon_server PRIVATE wsock32 ws2_32)
endif() 