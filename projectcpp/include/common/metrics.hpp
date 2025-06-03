#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <vector>

namespace perfmon {

struct SystemMetrics {
    std::chrono::system_clock::time_point timestamp;
    double cpu_usage;          // Percentage (0-100)
    double memory_usage;       // Percentage (0-100)
    uint64_t total_memory;     // Total physical memory in bytes
    uint64_t used_memory;      // Used physical memory in bytes
    uint64_t disk_read_bytes;  // Bytes read from disk since last measurement
    uint64_t disk_write_bytes; // Bytes written to disk since last measurement
    std::string hostname;      // Machine identifier

    // Serialization methods
    std::vector<char> serialize() const;
    static SystemMetrics deserialize(const std::vector<char>& data);
};

class MetricsCollector {
public:
    MetricsCollector();
    SystemMetrics collect();

private:
    double get_cpu_usage();
    void get_memory_info(uint64_t& total, uint64_t& used);
    void get_disk_io(uint64_t& read_bytes, uint64_t& write_bytes);
    std::string get_hostname();
};

} // namespace perfmon 