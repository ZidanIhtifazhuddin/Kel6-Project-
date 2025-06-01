#include "server.hpp"
#include <iostream>
#include <filesystem>
#include <csignal>

using namespace perfmon;

namespace {
    std::unique_ptr<Server> server;
    
    void signal_handler(int signal) {
        if (server) {
            std::cout << "\nShutting down server..." << std::endl;
            server->stop();
        }
        exit(signal);
    }
}

int main(int argc, char* argv[]) {
    try {
        // Set up signal handling
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

        // Default configuration
        uint16_t port = 12345;
        std::string data_dir = "data";

        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--port" && i + 1 < argc) {
                port = static_cast<uint16_t>(std::stoi(argv[++i]));
            }
            else if (arg == "--data-dir" && i + 1 < argc) {
                data_dir = argv[++i];
            }
        }

        // Create data directory if it doesn't exist
        std::filesystem::create_directories(data_dir);

        // Create and start server
        server = std::make_unique<Server>(port, data_dir);
        server->start();

        // Wait for signal
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 