# 🚀 Custom HTTP Server

A high-performance, event-driven HTTP server built in C++17 with automatic platform detection for optimal performance on both macOS and Linux.

## 📋 Prerequisites

- **C++17** compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- **CMake 3.10** or higher
- **Supported Operating Systems**:
  - **Linux** (x86_64, ARM64)
  - **macOS** (Intel, Apple Silicon)
  - **BSD variants** (FreeBSD, NetBSD, OpenBSD, DragonFly BSD)

## 🛠️ Building

### Clone the repository
```bash
git clone <repository-url>
cd http-server
```

### Build with CMake
```bash
mkdir build
cd build
cmake ..
make
```

The build system automatically detects your platform and configures the appropriate event loop implementation:
- **Linux**: Uses epoll for high-performance event handling
- **macOS/BSD**: Uses kqueue for efficient event processing

### Run the server
```bash
# Run on default port 8080
./http-server

# Run on custom port
./http-server 3000
```

The server will start on the specified port or default to 8080 if no port is provided.

## ⚙️ Configuration

### Port Configuration
The server port can be specified as a command-line argument:

```bash
./http-server 9000  # Start on port 9000
./http-server        # Start on default port 8080
```

### Adding New Routes
To add new endpoints, modify the router setup in `src/main.cpp`:

```cpp
Router router;
router.add_route("GET", "/new-endpoint", your_handler_function);
router.add_route("POST", "/api/data", post_handler_function);
```

### Custom Route Handlers
Route handlers follow this signature:

```cpp
void your_handler(const Request& req, Response& resp) {
    // Set response headers
    resp.set_header("Content-Type", "application/json");
    
    // Set response body
    resp.set_body("Your response content");
}
```

## 🚀 Performance Features

- **Cross-Platform Event-Driven I/O**: 
  - **Linux**: epoll-based event loop for maximum performance
  - **macOS/BSD**: kqueue-based event loop for optimal efficiency
- **Automatic Platform Detection**: Builds with the best event loop for your system
- **Non-blocking Operations**: Handles multiple connections concurrently
- **Memory Efficient**: Smart pointer management and proper resource cleanup
- **HTTP Message Parsing**: Efficient parsing with proper boundary detection
- **Connection Pooling**: Automatic connection lifecycle management

## 🔍 Technical Details

### HTTP Parsing
- Full HTTP/1.1 request parsing
- Header normalization (case-insensitive)
- Proper CRLF handling
- Content-Length support for request bodies

### Network Layer
- BSD socket implementation
- **Platform-optimized event loops**:
  - **Linux**: epoll for high-concurrency scenarios
  - **macOS/BSD**: kqueue for efficient event processing
- Proper error handling and resource cleanup
- Connection state management

## 🧪 Testing

### Load Testing
```bash
# Simple load test with Apache Bench
ab -n 1000 -c 10 http://localhost:8080/
```

## 📦 Dependencies

- **Standard Library**: C++17 standard library
- **System Libraries**: 
  - `<sys/socket.h>` - BSD sockets
  - **Linux**: `<sys/epoll.h>` - epoll events
  - **macOS/BSD**: `<sys/event.h>` - kqueue events
  - `<netinet/in.h>` - Internet address structures
  - `<unistd.h>` - POSIX system calls

### Development Guidelines
- Follow C++17 best practices
- Maintain consistent code style
- Add appropriate error handling
- Include comments for complex logic
- Test thoroughly before submitting

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with modern C++17 features
- Inspired by high-performance web servers like nginx
- **Cross-platform design** with automatic platform detection
- Uses epoll (Linux) and kqueue (macOS/BSD) for optimal performance on each platform

---
