#include "../../include/http/router.hpp"

Router::Router() {
}

Router::~Router() {
}

void Router::add_route(const std::string& method, const std::string& path, Handler handler) {
    routes_[method + path] = std::move(handler);
}

Response Router::route(const Request& req) const {
    auto it = routes_.find(req.method + req.path);
    if (it != routes_.end()) {
        Response resp(200, "OK");
        std::cout << "Route found for " << req.method << " " << req.path << std::endl;
        it->second(req, resp);
        return resp;
    }
    std::cout << "Route not found for " << req.method << " " << req.path << std::endl;
    return Response(404, "Not Found");
}

void Router::handle_request(Request& request, Response& response) {
    response = Response(404, "Not Found", "Route not found");
} 