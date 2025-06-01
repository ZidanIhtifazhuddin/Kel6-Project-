#include "client.hpp"
#include <iostream>
#include <csignal>

using namespace perfmon;

namespace {
    std::unique_ptr<Client> client;
    
    void signal_handler(int signal) {
        if (client) {
            std::cout << "\nShutting down client..." << std::endl;
            client->stop();
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
        std::string server_address = "127.0.0.1";
        uint16_t server_port = 12345;
        int interval = 1;

        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--server" && i + 1 < argc) {
                server_address = argv[++i];
            }
            else if (arg == "--port" && i + 1 < argc) {
                server_port = static_cast<uint16_t>(std::stoi(argv[++i]));
            }
            else if (arg == "--interval" && i + 1 < argc) {
                interval = std::stoi(argv[++i]);
            }
        }

        // Create and start client
        client = std::make_unique<Client>(
            server_address,
            server_port,
            std::chrono::seconds(interval)
        );
        
        client->start();

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