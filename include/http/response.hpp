#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <unordered_map>

class Response {
public:
    Response(int code, std::string msg, std::string body = "")
        : status_code(code), status_message(std::move(msg)), body(std::move(body)) {
        headers["Content-Length"] = std::to_string(this->body.size());
        headers["Content-Type"] = "text/plain";
    }

    void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    void set_body(const std::string& new_body) {
        body = new_body;
        headers["Content-Length"] = std::to_string(body.size());
    }

    std::string to_string() const {
        std::string response;
        response += "HTTP/1.1 " + std::to_string(status_code) + " " + status_message + "\r\n";
        for (const auto& header : headers) {
            response += header.first + ": " + header.second + "\r\n";
        }
        response += "\r\n" + body;
        return response;
    }

    std::string get_status_line() const {
        return "HTTP/1.1 " + std::to_string(status_code) + " " + status_message;
    }

private:
    int status_code;
    std::string status_message;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

#endif
