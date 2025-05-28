---
title: common.h

---

- Windows OSServer.cpp
```cpp
// aji bau
// oaufjaifjijbfi
// oajdjsjdia
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>    // For std::put_time, std::get_time
#include <sstream>    // For string stream manipulations
#include <fstream>    // For file operations
#include <algorithm>  // For std::sort, std::copy_if, std::min
#include <thread>
#include <mutex>
#include <random>     // For generating mock data
#include <functional> // For std::function
#include <list>       // For demo_linked_list
#include <stack>      // For demo_stack_queue
#include <queue>      // For demo_stack_queue

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h> // For close, fcntl
    #include <fcntl.h>  // For fcntl
    typedef int SOCKET;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
#endif

const int BUFFER_SIZE = 1024;
const std::string BINARY_LOG_FILE = "performance_log.bin";
// Nama file JSON default, bisa diganti saat pemanggilan fungsi ekspor
// const std::string JSON_EXPORT_FILE = "export_data.json";

// Struktur untuk menyimpan metrik performa
struct PerformanceMetric {
    std::string hostname;
    std::chrono::system_clock::time_point timestamp;
    double cpuUsage;       // Persentase
    double memoryUsage;    // Persentase
    double diskIOActiveTime; // Persentase waktu disk aktif (simulasi)

    // Untuk serialisasi/deserialisasi sederhana ke/dari string
    std::string toString() const {
        std::stringstream ss;
        ss << hostname << "|"
           << std::chrono::system_clock::to_time_t(timestamp) << "|"
           << cpuUsage << "|"
           << memoryUsage << "|"
           << diskIOActiveTime;
        return ss.str();
    }

    static PerformanceMetric fromString(const std::string& s) {
        PerformanceMetric metric;
        std::stringstream ss(s);
        std::string segment;
        
        std::getline(ss, segment, '|'); metric.hostname = segment;
        std::getline(ss, segment, '|'); metric.timestamp = std::chrono::system_clock::from_time_t(std::stoll(segment)); // string to long long
        std::getline(ss, segment, '|'); metric.cpuUsage = std::stod(segment);    // string to double
        std::getline(ss, segment, '|'); metric.memoryUsage = std::stod(segment); // string to double
        std::getline(ss, segment, '|'); metric.diskIOActiveTime = std::stod(segment); // string to double
        
        return metric;
    }
};

// Overload untuk output stream (untuk file biner)
inline std::ostream& operator<<(std::ostream& os, const PerformanceMetric& metric) {
    size_t hostname_len = metric.hostname.length();
    os.write(reinterpret_cast<const char*>(&hostname_len), sizeof(hostname_len));
    os.write(metric.hostname.c_str(), hostname_len);
    
    auto time_t_val = std::chrono::system_clock::to_time_t(metric.timestamp);
    os.write(reinterpret_cast<const char*>(&time_t_val), sizeof(time_t_val));
    os.write(reinterpret_cast<const char*>(&metric.cpuUsage), sizeof(metric.cpuUsage));
    os.write(reinterpret_cast<const char*>(&metric.memoryUsage), sizeof(metric.memoryUsage));
    os.write(reinterpret_cast<const char*>(&metric.diskIOActiveTime), sizeof(metric.diskIOActiveTime));
    return os;
}

// Overload untuk input stream (untuk file biner)
inline std::istream& operator>>(std::istream& is, PerformanceMetric& metric) {
    size_t hostname_len;
    is.read(reinterpret_cast<char*>(&hostname_len), sizeof(hostname_len));
    if (is.gcount() != sizeof(hostname_len)) return is; // Check read success

    metric.hostname.resize(hostname_len);
    is.read(&metric.hostname[0], hostname_len);
    if (is.gcount() != hostname_len) return is;

    std::time_t time_t_val;
    is.read(reinterpret_cast<char*>(&time_t_val), sizeof(time_t_val));
    if (is.gcount() != sizeof(time_t_val)) return is;
    metric.timestamp = std::chrono::system_clock::from_time_t(time_t_val);

    is.read(reinterpret_cast<char*>(&metric.cpuUsage), sizeof(metric.cpuUsage));
    if (is.gcount() != sizeof(metric.cpuUsage)) return is;
    is.read(reinterpret_cast<char*>(&metric.memoryUsage), sizeof(metric.memoryUsage));
    if (is.gcount() != sizeof(metric.memoryUsage)) return is;
    is.read(reinterpret_cast<char*>(&metric.diskIOActiveTime), sizeof(metric.diskIOActiveTime));
    if (is.gcount() != sizeof(metric.diskIOActiveTime)) return is;

    return is;
}

// Helper untuk format timestamp
inline std::string format_timestamp(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_info;
    #ifdef _WIN32
        localtime_s(&tm_info, &t); // Windows specific
    #else
        localtime_r(&t, &tm_info); // POSIX specific
    #endif
    std::stringstream ss;
    ss << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Function pointer type (std::function lebih fleksibel)
using CompareMetricFuncStd = std::function<bool(const PerformanceMetric&, const PerformanceMetric&)>;


// Contoh penggunaan Stack & Queue sederhana
inline void demo_stack_queue() {
    std::cout << "\n--- Demo Stack & Queue ---" << std::endl;
    std::stack<int> myStack;
    myStack.push(1); myStack.push(2); myStack.push(3);
    if (!myStack.empty()) { std::cout << "Stack top: " << myStack.top(); myStack.pop(); }
    if (!myStack.empty()) { std::cout << ", After pop: " << myStack.top() << std::endl; }
    else { std::cout << std::endl; }


    std::queue<std::string> myQueue;
    myQueue.push("Alpha"); myQueue.push("Beta");
    if(!myQueue.empty()){ std::cout << "Queue front: " << myQueue.front(); myQueue.pop(); }
    if(!myQueue.empty()){ std::cout << ", After pop: " << myQueue.front() << std::endl; }
    else { std::cout << std::endl; }
    std::cout << "--- End Demo Stack & Queue ---\n" << std::endl;
}

// Contoh penggunaan Linked List (std::list)
inline void demo_linked_list() {
    std::cout << "\n--- Demo std::list (Linked List) ---" << std::endl;
    std::list<PerformanceMetric> metricList;
    // Tambah elemen
    metricList.push_back({ "LL-PC1", std::chrono::system_clock::now(), 10.0, 20.0, 5.0 });
    metricList.push_front({ "LL-PC2", std::chrono::system_clock::now(), 15.0, 25.0, 8.0 });
    
    std::cout << "Linked List contents:" << std::endl;
    for (const auto& m : metricList) {
        std::cout << "  " << m.hostname << " CPU: " << m.cpuUsage << "% @ " << format_timestamp(m.timestamp) << std::endl;
    }
    std::cout << "--- End Demo std::list ---\n" << std::endl;
}

#endif // COMMON_H    
```
