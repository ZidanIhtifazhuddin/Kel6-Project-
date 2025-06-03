#pragma once

#include "common/metrics.hpp"
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <fstream>

namespace perfmon {

class DataStore {
public:
    explicit DataStore(const std::string& data_dir);

    void store_metrics(const SystemMetrics& metrics);
    std::vector<SystemMetrics> get_metrics(
        const std::string& hostname,
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end);
    
    void export_json(
        const std::string& filename,
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end);

private:
    std::string get_binary_filename(const std::string& hostname) const;
    void append_to_file(const std::string& filename, const SystemMetrics& metrics);
    std::vector<SystemMetrics> read_from_file(
        const std::string& filename,
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end);

    std::string data_dir_;
    std::mutex store_mutex_;
};

} // namespace perfmon 