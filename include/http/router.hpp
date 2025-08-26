#include <functional>
#include <unordered_map>
#include <string>
#include <cstddef>
#include "request.hpp"
#include "response.hpp"
#include <iostream>

typedef void (*Handler)(const Request&, Response&);

class Router {
public:
    void add_route(const std::string& method, const std::string& path, Handler handler) {
        routes_[method + path] = std::move(handler);
    }

    Response route(const Request& req) const {
        auto it = routes_.find(req.method + req.path);
        if (it != routes_.end()) {
            Response resp(200, "OK"); // Create response with default success values
            std::cout << "Route found for " << req.method << " " << req.path << std::endl;
            it->second(req, resp); // call handler with both request and response
            return resp;
        }
        std::cout << "Route not found for " << req.method << " " << req.path << std::endl;
        return Response(404, "Not Found"); // default 404
    }

private:
    std::unordered_map<std::string, Handler> routes_;
};
