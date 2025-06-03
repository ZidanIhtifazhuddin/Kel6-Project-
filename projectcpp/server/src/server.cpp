#include "server.hpp"
#include <iostream>
#include <filesystem>

namespace perfmon {

Server::Server(uint16_t port, const std::string& data_dir)
    : port_(port)
    , server_socket_(std::make_unique<Socket>())
    , data_store_(std::make_unique<DataStore>(data_dir))
    , running_(false) {
}

Server::~Server() {
    stop();
}

void Server::start() {
    if (running_) {
        return;
    }

    try {
        server_socket_->bind("", port_);
        server_socket_->listen();
        running_ = true;

        std::cout << "Server started on port " << port_ << std::endl;
        
        // Start accepting connections in a separate thread
        client_threads_.emplace_back(&Server::accept_connections, this);
    }
    catch (const SocketException& e) {
        std::cerr << "Failed to start server: " << e.what() << std::endl;
        throw;
    }
}

void Server::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    // Wait for all client threads to finish
    for (auto& thread : client_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    client_threads_.clear();
}

void Server::accept_connections() {
    while (running_) {
        try {
            auto client_socket = server_socket_->accept();
            std::cout << "New client connected" << std::endl;

            // Start a new thread to handle the client
            std::lock_guard<std::mutex> lock(clients_mutex_);
            client_threads_.emplace_back(&Server::handle_client, this, std::move(client_socket));
        }
        catch (const SocketException& e) {
            if (running_) {
                std::cerr << "Error accepting client connection: " << e.what() << std::endl;
            }
        }
    }
}

void Server::handle_client(std::unique_ptr<Socket> client_socket) {
    try {
        while (running_) {
            // Receive metrics data from client
            auto data = client_socket->receive();
            if (data.empty()) {
                break; // Client disconnected
            }

            // Deserialize the metrics
            auto metrics = SystemMetrics::deserialize(data);
            
            // Process and store the metrics
            process_metrics(metrics);
        }
    }
    catch (const SocketException& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }

    std::cout << "Client disconnected" << std::endl;
}

void Server::process_metrics(const SystemMetrics& metrics) {
    try {
        // Store the metrics in the data store
        data_store_->store_metrics(metrics);

        // Log the received metrics
        std::cout << "Received metrics from " << metrics.hostname 
                  << " - CPU: " << metrics.cpu_usage << "%, "
                  << "Memory: " << metrics.memory_usage << "%" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error processing metrics: " << e.what() << std::endl;
    }
}

} // namespace perfmon 