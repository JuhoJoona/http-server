#include "../../include/http/parser.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace HttpParser {

bool parseRequestLine(const std::string& line, Request& request) {
    std::vector<std::string> parts = split(trim(line), ' ');
    
    if (parts.size() < 2) {
        return false;
    }
    
    request.method = parts[0];
    request.path = parts[1];
    
    // HTTP version is optional in the third part
    if (parts.size() >= 3) {
        // Could store version if needed
    }
    
    return true;
}

bool parseHeader(const std::string& line, Request& request) {
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
        return false;
    }
    
    std::string key = trim(line.substr(0, colonPos));
    std::string value = trim(line.substr(colonPos + 1));
    
    // Convert header names to lowercase for consistency
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    
    request.headers[key] = value;
    return true;
}

bool parseRequest(const std::string& rawData, Request& request) {
    std::istringstream stream(rawData);
    std::string line;
    
    // Parse request line (first line)
    if (!std::getline(stream, line)) {
        return false;
    }
    
    // Remove carriage return if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    
    if (!parseRequestLine(line, request)) {
        return false;
    }
    
    // Parse headers
    while (std::getline(stream, line)) {
        // Remove carriage return if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // Empty line separates headers from body
        if (line.empty()) {
            break;
        }
        
        parseHeader(line, request);
    }
    
    // Parse body (everything after the empty line)
    std::string body;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!body.empty()) {
            body += "\n";
        }
        body += line;
    }
    
    request.body = body;
    
    return true;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

} 