#pragma once

#include "common/socket_wrapper.hpp"
#include "common/metrics.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

namespace perfmon {

class Client {
public:
    Client(const std::string& server_address, uint16_t server_port,
           std::chrono::seconds collection_interval = std::chrono::seconds(1));
    ~Client();

    // Prevent copying
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void start();
    void stop();

private:
    void collect_and_send_metrics();

    std::string server_address_;
    uint16_t server_port_;
    std::chrono::seconds collection_interval_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<MetricsCollector> collector_;
    std::thread collection_thread_;
    std::atomic<bool> running_;
};

} // namespace perfmon 