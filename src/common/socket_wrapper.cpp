#include "common/socket_wrapper.hpp"
#include <system_error>

namespace perfmon {

bool Socket::wsa_initialized_ = false;

void Socket::init_wsa() {
#ifdef _WIN32
    if (!wsa_initialized_) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw SocketException("Failed to initialize WinSock");
        }
        wsa_initialized_ = true;
    }
#endif
}

Socket::Socket() : sock_(INVALID_SOCKET), connected_(false) {
    init_wsa();
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ == INVALID_SOCKET) {
        throw SocketException("Failed to create socket");
    }
}

Socket::~Socket() {
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
    }
#ifdef _WIN32
    if (wsa_initialized_) {
        WSACleanup();
        wsa_initialized_ = false;
    }
#endif
}

Socket::Socket(Socket&& other) noexcept
    : sock_(other.sock_), connected_(other.connected_) {
    other.sock_ = INVALID_SOCKET;
    other.connected_ = false;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        if (sock_ != INVALID_SOCKET) {
            closesocket(sock_);
        }
        sock_ = other.sock_;
        connected_ = other.connected_;
        other.sock_ = INVALID_SOCKET;
        other.connected_ = false;
    }
    return *this;
}

void Socket::bind(const std::string& address, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = address.empty() ? INADDR_ANY : inet_addr(address.c_str());

    if (::bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        throw SocketException("Failed to bind socket");
    }
}

void Socket::listen(int backlog) {
    if (::listen(sock_, backlog) == SOCKET_ERROR) {
        throw SocketException("Failed to listen on socket");
    }
}

std::unique_ptr<Socket> Socket::accept() {
    SOCKET client_sock = ::accept(sock_, nullptr, nullptr);
    if (client_sock == INVALID_SOCKET) {
        throw SocketException("Failed to accept connection");
    }

    auto client_socket = std::make_unique<Socket>();
    client_socket->sock_ = client_sock;
    client_socket->connected_ = true;
    return client_socket;
}

void Socket::connect(const std::string& address, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.c_str());

    if (::connect(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        throw SocketException("Failed to connect");
    }
    connected_ = true;
}

size_t Socket::send(const std::vector<char>& data) {
    int result = ::send(sock_, data.data(), static_cast<int>(data.size()), 0);
    if (result == SOCKET_ERROR) {
        throw SocketException("Failed to send data");
    }
    return static_cast<size_t>(result);
}

std::vector<char> Socket::receive(size_t max_size) {
    std::vector<char> buffer(max_size);
    int result = recv(sock_, buffer.data(), static_cast<int>(buffer.size()), 0);
    
    if (result == SOCKET_ERROR) {
        throw SocketException("Failed to receive data");
    }
    
    buffer.resize(result);
    return buffer;
}

} // namespace perfmon 