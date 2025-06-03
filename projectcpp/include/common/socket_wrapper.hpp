#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

namespace perfmon {

class SocketException : public std::runtime_error {
public:
    explicit SocketException(const std::string& message) : std::runtime_error(message) {}
};

class Socket {
public:
    Socket();
    ~Socket();

    // Prevent copying
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // Allow moving
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void bind(const std::string& address, uint16_t port);
    void listen(int backlog = 10);
    std::unique_ptr<Socket> accept();
    void connect(const std::string& address, uint16_t port);
    
    size_t send(const std::vector<char>& data);
    std::vector<char> receive(size_t max_size = 4096);

private:
    SOCKET sock_;
    bool connected_;

    static bool wsa_initialized_;
    static void init_wsa();
};

} // namespace perfmon 