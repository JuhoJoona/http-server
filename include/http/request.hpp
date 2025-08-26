#include <string>
#include <unordered_map>

#ifndef REQUEST_HPP
#define REQUEST_HPP

struct Request {
    std::string method;   // "GET", "POST", ...
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

#endif
