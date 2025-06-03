#include "data_store.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace perfmon {

DataStore::DataStore(const std::string& data_dir) : data_dir_(data_dir) {
    std::filesystem::create_directories(data_dir);
}

void DataStore::store_metrics(const SystemMetrics& metrics) {
    std::lock_guard<std::mutex> lock(store_mutex_);
    
    std::string filename = get_binary_filename(metrics.hostname);
    append_to_file(filename, metrics);
}

std::vector<SystemMetrics> DataStore::get_metrics(
    const std::string& hostname,
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) {
    
    std::lock_guard<std::mutex> lock(store_mutex_);
    std::string filename = get_binary_filename(hostname);
    return read_from_file(filename, start, end);
}

void DataStore::export_json(
    const std::string& filename,
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) {
    
    std::lock_guard<std::mutex> lock(store_mutex_);
    
    // Collect metrics from all host files
    std::vector<SystemMetrics> all_metrics;
    for (const auto& entry : std::filesystem::directory_iterator(data_dir_)) {
        if (entry.path().extension() == ".bin") {
            auto host_metrics = read_from_file(entry.path().string(), start, end);
            all_metrics.insert(all_metrics.end(), host_metrics.begin(), host_metrics.end());
        }
    }
    
    // Sort metrics by timestamp
    std::sort(all_metrics.begin(), all_metrics.end(),
              [](const SystemMetrics& a, const SystemMetrics& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Write JSON file
    std::ofstream json_file(filename);
    json_file << "{\n  \"metrics\": [\n";
    
    for (size_t i = 0; i < all_metrics.size(); ++i) {
        const auto& m = all_metrics[i];
        auto time_t = std::chrono::system_clock::to_time_t(m.timestamp);
        
        json_file << "    {\n"
                 << "      \"timestamp\": \"" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\",\n"
                 << "      \"hostname\": \"" << m.hostname << "\",\n"
                 << "      \"cpu_usage\": " << m.cpu_usage << ",\n"
                 << "      \"memory_usage\": " << m.memory_usage << ",\n"
                 << "      \"total_memory\": " << m.total_memory << ",\n"
                 << "      \"used_memory\": " << m.used_memory << ",\n"
                 << "      \"disk_read_bytes\": " << m.disk_read_bytes << ",\n"
                 << "      \"disk_write_bytes\": " << m.disk_write_bytes << "\n"
                 << "    }" << (i < all_metrics.size() - 1 ? "," : "") << "\n";
    }
    
    json_file << "  ]\n}\n";
}

std::string DataStore::get_binary_filename(const std::string& hostname) const {
    return data_dir_ + "/" + hostname + ".bin";
}

void DataStore::append_to_file(const std::string& filename, const SystemMetrics& metrics) {
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    
    auto data = metrics.serialize();
    file.write(data.data(), data.size());
}

std::vector<SystemMetrics> DataStore::read_from_file(
    const std::string& filename,
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) {
    
    std::vector<SystemMetrics> metrics;
    
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return metrics; // Return empty vector if file doesn't exist
    }
    
    // Read file in chunks
    std::vector<char> buffer(4096);
    std::vector<char> data;
    
    while (file.read(buffer.data(), buffer.size())) {
        data.insert(data.end(), buffer.begin(), buffer.begin() + file.gcount());
    }
    if (file.gcount() > 0) {
        data.insert(data.end(), buffer.begin(), buffer.begin() + file.gcount());
    }
    
    // Process the data
    size_t pos = 0;
    while (pos < data.size()) {
        std::vector<char> metric_data(data.begin() + pos, data.end());
        auto metric = SystemMetrics::deserialize(metric_data);
        
        if (metric.timestamp >= start && metric.timestamp <= end) {
            metrics.push_back(metric);
        }
        
        pos += metric_data.size();
    }
    
    return metrics;
}

} // namespace perfmon 