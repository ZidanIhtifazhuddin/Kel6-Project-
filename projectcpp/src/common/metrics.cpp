#include "common/metrics.hpp"
#include <windows.h>
#include <pdh.h>
#include <memory>
#include <cstring>

namespace perfmon {

std::vector<char> SystemMetrics::serialize() const {
    std::vector<char> buffer;
    auto timestamp_val = timestamp.time_since_epoch().count();
    
    // Calculate total size needed
    size_t total_size = sizeof(timestamp_val) + 
                       sizeof(cpu_usage) + 
                       sizeof(memory_usage) +
                       sizeof(total_memory) +
                       sizeof(used_memory) +
                       sizeof(disk_read_bytes) +
                       sizeof(disk_write_bytes) +
                       sizeof(size_t) +  // For hostname length
                       hostname.size();
    
    buffer.resize(total_size);
    char* ptr = buffer.data();
    
    // Copy data
    std::memcpy(ptr, &timestamp_val, sizeof(timestamp_val));
    ptr += sizeof(timestamp_val);
    
    std::memcpy(ptr, &cpu_usage, sizeof(cpu_usage));
    ptr += sizeof(cpu_usage);
    
    std::memcpy(ptr, &memory_usage, sizeof(memory_usage));
    ptr += sizeof(memory_usage);
    
    std::memcpy(ptr, &total_memory, sizeof(total_memory));
    ptr += sizeof(total_memory);
    
    std::memcpy(ptr, &used_memory, sizeof(used_memory));
    ptr += sizeof(used_memory);
    
    std::memcpy(ptr, &disk_read_bytes, sizeof(disk_read_bytes));
    ptr += sizeof(disk_read_bytes);
    
    std::memcpy(ptr, &disk_write_bytes, sizeof(disk_write_bytes));
    ptr += sizeof(disk_write_bytes);
    
    size_t hostname_size = hostname.size();
    std::memcpy(ptr, &hostname_size, sizeof(hostname_size));
    ptr += sizeof(hostname_size);
    
    std::memcpy(ptr, hostname.data(), hostname_size);
    
    return buffer;
}

SystemMetrics SystemMetrics::deserialize(const std::vector<char>& data) {
    SystemMetrics metrics;
    const char* ptr = data.data();
    
    // Read timestamp
    typename std::chrono::system_clock::duration::rep timestamp_val;
    std::memcpy(&timestamp_val, ptr, sizeof(timestamp_val));
    ptr += sizeof(timestamp_val);
    metrics.timestamp = std::chrono::system_clock::time_point(
        std::chrono::system_clock::duration(timestamp_val));
    
    // Read other fields
    std::memcpy(&metrics.cpu_usage, ptr, sizeof(metrics.cpu_usage));
    ptr += sizeof(metrics.cpu_usage);
    
    std::memcpy(&metrics.memory_usage, ptr, sizeof(metrics.memory_usage));
    ptr += sizeof(metrics.memory_usage);
    
    std::memcpy(&metrics.total_memory, ptr, sizeof(metrics.total_memory));
    ptr += sizeof(metrics.total_memory);
    
    std::memcpy(&metrics.used_memory, ptr, sizeof(metrics.used_memory));
    ptr += sizeof(metrics.used_memory);
    
    std::memcpy(&metrics.disk_read_bytes, ptr, sizeof(metrics.disk_read_bytes));
    ptr += sizeof(metrics.disk_read_bytes);
    
    std::memcpy(&metrics.disk_write_bytes, ptr, sizeof(metrics.disk_write_bytes));
    ptr += sizeof(metrics.disk_write_bytes);
    
    size_t hostname_size;
    std::memcpy(&hostname_size, ptr, sizeof(hostname_size));
    ptr += sizeof(hostname_size);
    
    metrics.hostname.assign(ptr, ptr + hostname_size);
    
    return metrics;
}

MetricsCollector::MetricsCollector() {
    // Initialize PDH
    PdhOpenQuery(nullptr, 0, nullptr);
}

SystemMetrics MetricsCollector::collect() {
    SystemMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.cpu_usage = get_cpu_usage();
    get_memory_info(metrics.total_memory, metrics.used_memory);
    metrics.memory_usage = (static_cast<double>(metrics.used_memory) / metrics.total_memory) * 100.0;
    get_disk_io(metrics.disk_read_bytes, metrics.disk_write_bytes);
    metrics.hostname = get_hostname();
    return metrics;
}

double MetricsCollector::get_cpu_usage() {
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    
    static ULARGE_INTEGER lastIdleTime = {0};
    static ULARGE_INTEGER lastKernelTime = {0};
    static ULARGE_INTEGER lastUserTime = {0};
    
    ULARGE_INTEGER idle, kernel, user;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;
    
    if (lastIdleTime.QuadPart != 0) {
        ULONGLONG idleDiff = idle.QuadPart - lastIdleTime.QuadPart;
        ULONGLONG kernelDiff = kernel.QuadPart - lastKernelTime.QuadPart;
        ULONGLONG userDiff = user.QuadPart - lastUserTime.QuadPart;
        ULONGLONG totalDiff = kernelDiff + userDiff;
        
        if (totalDiff > 0) {
            return ((totalDiff - idleDiff) * 100.0) / totalDiff;
        }
    }
    
    lastIdleTime = idle;
    lastKernelTime = kernel;
    lastUserTime = user;
    
    return 0.0;
}

void MetricsCollector::get_memory_info(uint64_t& total, uint64_t& used) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    total = memInfo.ullTotalPhys;
    used = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
}

void MetricsCollector::get_disk_io(uint64_t& read_bytes, uint64_t& write_bytes) {
    static ULARGE_INTEGER lastReadBytes = {0};
    static ULARGE_INTEGER lastWriteBytes = {0};
    
    ULARGE_INTEGER readBytes, writeBytes;
    GetSystemTimeAsFileTime((FILETIME*)&readBytes);  // Using as a placeholder
    GetSystemTimeAsFileTime((FILETIME*)&writeBytes); // Using as a placeholder
    
    // In a real implementation, you would use GetDiskIoCounters or similar
    read_bytes = readBytes.QuadPart - lastReadBytes.QuadPart;
    write_bytes = writeBytes.QuadPart - lastWriteBytes.QuadPart;
    
    lastReadBytes = readBytes;
    lastWriteBytes = writeBytes;
}

std::string MetricsCollector::get_hostname() {
    char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(hostname);
    GetComputerNameA(hostname, &size);
    return std::string(hostname);
}

} // namespace perfmon 