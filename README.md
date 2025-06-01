# Performance Monitoring System
# KELOMPOK 6
A client-server system for monitoring and analyzing computer performance metrics in a university computer lab environment.

## Features

- Real-time performance metrics collection (CPU, memory, disk I/O)
- Multi-client support with thread-safe data handling
- Binary data storage for efficiency
- JSON export capability for analysis
- Cross-platform socket communication (Windows/Unix)

## Installation (Instalasi)

### Prerequisites (Prasyarat)

- CMake 3.10 atau lebih tinggi
- Compiler yang mendukung C++17
- Windows: Visual Studio dengan workload C++
- Unix: GCC/Clang

### Langkah-langkah Instalasi

1. Pastikan Anda telah menginstal Visual Studio 2022:
   - Download Visual Studio 2022 dari [visualstudio.microsoft.com](https://visualstudio.microsoft.com)
   - Pilih "Desktop development with C++" saat instalasi
   - Pastikan komponen "Windows SDK" terpilih

2. Instal CMake:
   - Download CMake dari [cmake.org](https://cmake.org/download)
   - Pilih versi Windows x64 Installer
   - Saat instalasi, pilih opsi "Add CMake to the system PATH"

3. Clone atau download repository ini ke komputer Anda

4. Buka PowerShell atau Command Prompt dan navigasi ke direktori proyek

5. Buat direktori build dan compile proyek:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

## Cara Penggunaan

### Menjalankan Server

1. Buka terminal baru
2. Navigasi ke direktori proyek
3. Jalankan server dengan perintah:
```bash
.\build\server\Debug\perfmon_server.exe 12345 data
```
   - Port 12345 adalah port default untuk server
   - Direktori 'data' akan dibuat untuk menyimpan metrik

### Menjalankan Client

1. Buka terminal baru
2. Navigasi ke direktori proyek
3. Jalankan client dengan perintah:
```bash
.\build\client\Debug\perfmon_client.exe 127.0.0.1 12345 1
```
   - 127.0.0.1 adalah alamat server (localhost)
   - 12345 adalah port server
   - 1 adalah interval pengiriman data dalam detik

### Memverifikasi Sistem Berjalan

1. Server akan menampilkan pesan bahwa sedang mendengarkan koneksi
2. Client akan menampilkan pesan bahwa telah terhubung ke server
3. Direktori 'data' akan dibuat dengan file .bin untuk setiap client
4. Server akan menampilkan pembaruan periodik tentang metrik yang diterima

### Menghentikan Sistem

1. Tekan Ctrl+C di kedua terminal untuk menghentikan server dan client
2. Atau tutup terminal untuk menghentikan program

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- Windows: Visual Studio with C++ workload
- Unix: GCC/Clang

### Build Steps

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure with CMake:
```bash
cmake ..
```

3. Build:
```bash
cmake --build .
```

## Usage

### Server

The server collects and stores metrics from multiple clients. Run it with:

```bash
./perfmon_server [options]

Options:
  --port PORT        Server port (default: 12345)
  --data-dir DIR    Data storage directory (default: "data")
```

### Client

The client runs on each computer to be monitored. Run it with:

```bash
./perfmon_client [options]

Options:
  --server HOST     Server address (default: "127.0.0.1")
  --port PORT       Server port (default: 12345)
  --interval SECS   Collection interval in seconds (default: 1)
```

### Data Export

The server stores data in binary format for efficiency. To analyze the data:

1. Data is stored in the specified data directory with one file per client machine
2. Each file is named after the client's hostname with a .bin extension
3. Use the JSON export functionality to convert binary data to a readable format

## Architecture

### Components

1. Common Library
   - Metrics collection and serialization
   - Socket communication wrapper
   - Cross-platform compatibility layer

2. Server
   - Multi-threaded client handling
   - Thread-safe data storage
   - Binary and JSON data formats

3. Client
   - Periodic metrics collection
   - Reliable data transmission
   - Configurable collection interval

### Data Flow

1. Client collects system metrics at specified intervals
2. Metrics are serialized and sent to server
3. Server deserializes and stores data in binary format
4. Data can be exported to JSON for analysis

## Performance Considerations

- Binary format for efficient storage and network transmission
- Thread-safe operations for concurrent client connections
- Configurable collection intervals to manage system load
- Efficient socket communication with error handling

## Security Notes

- Basic error handling and input validation
- No built-in encryption (use VPN or similar for secure transmission)
- No authentication mechanism (implement as needed)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details. 