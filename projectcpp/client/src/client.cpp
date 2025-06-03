#include "client.hpp"
#include <iostream>

namespace perfmon {

Client::Client(const std::string& server_address, uint16_t server_port,
               std::chrono::seconds collection_interval)
    : server_address_(server_address)
    , server_port_(server_port)
    , collection_interval_(collection_interval)
    , socket_(std::make_unique<Socket>())
    , collector_(std::make_unique<MetricsCollector>())
    , running_(false) {
}

Client::~Client() {
    stop();
}

void Client::start() {
    if (running_) {
        return;
    }

    try {
        // Connect to server
        socket_->connect(server_address_, server_port_);
        running_ = true;

        std::cout << "Connected to server at " << server_address_ << ":" << server_port_ << std::endl;

        // Start metrics collection thread
        collection_thread_ = std::thread(&Client::collect_and_send_metrics, this);
    }
    catch (const SocketException& e) {
        std::cerr << "Failed to connect to server: " << e.what() << std::endl;
        throw;
    }
}

void Client::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    if (collection_thread_.joinable()) {
        collection_thread_.join();
    }
}

void Client::collect_and_send_metrics() {
    while (running_) {
        try {
            // Collect metrics
            auto metrics = collector_->collect();

            // Serialize and send metrics
            auto data = metrics.serialize();
            socket_->send(data);

            // Wait for the next collection interval
            std::this_thread::sleep_for(collection_interval_);
        }
        catch (const std::exception& e) {
            std::cerr << "Error collecting/sending metrics: " << e.what() << std::endl;
            running_ = false;
            break;
        }
    }
}

} // namespace perfmon 