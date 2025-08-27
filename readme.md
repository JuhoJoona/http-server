
## 📋 Prerequisites

- **C++17** compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- **CMake 3.10** or higher
- **Unix-like operating system** (macOS, Linux, BSD)

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

### Run the server
```bash
./http-server
```

The server will start on port 8080 by default.

```

## �� Configuration

### Port Configuration
The server port can be modified in `src/main.cpp`:

```cpp
Server server(8080, router);  // Change 8080 to your desired port
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

- **Event-Driven I/O**: Uses kqueue for efficient event handling
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
- kqueue event loop for scalability
- Proper error handling and resource cleanup
- Connection state management

## 🧪 Testing

### Load Testing
```bash
# Simple load test with Apache Bench
ab -n 1000 -c 10 http://localhost:8080/
```

## �� Dependencies

- **Standard Library**: C++17 standard library
- **System Libraries**: 
  - `<sys/socket.h>` - BSD sockets
  - `<sys/event.h>` - kqueue events
  - `<netinet/in.h>` - Internet address structures
  - `<unistd.h>` - POSIX system calls

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## �� Acknowledgments

- Built with modern C++17 features
- Uses kqueue for efficient event handling on Unix-like systems

---
