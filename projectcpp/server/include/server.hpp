#pragma once

#include "common/socket_wrapper.hpp"
#include "common/metrics.hpp"
#include "data_store.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

namespace perfmon {

class Server {
public:
    Server(uint16_t port, const std::string& data_dir);
    ~Server();

    // Prevent copying
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void start();
    void stop();

private:
    void accept_connections();
    void handle_client(std::unique_ptr<Socket> client_socket);
    void process_metrics(const SystemMetrics& metrics);

    uint16_t port_;
    std::unique_ptr<Socket> server_socket_;
    std::unique_ptr<DataStore> data_store_;
    std::vector<std::thread> client_threads_;
    std::atomic<bool> running_;
    std::mutex clients_mutex_;
};

} // namespace perfmon 