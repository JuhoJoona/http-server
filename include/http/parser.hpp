#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "request.hpp"
#include <string>
#include <sstream>
#include <algorithm>

namespace HttpParser {
    
    // Parse the request line (e.g., "GET /path HTTP/1.1")
    bool parseRequestLine(const std::string& line, Request& request);
    
    // Parse headers (e.g., "Content-Type: text/html")
    bool parseHeader(const std::string& line, Request& request);
    
    // Parse a complete HTTP request from raw data
    bool parseRequest(const std::string& rawData, Request& request);
    
    // Helper function to trim whitespace
    std::string trim(const std::string& str);
    
    // Helper function to split string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter);
}

#endif 