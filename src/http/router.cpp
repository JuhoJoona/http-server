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
        it->second(req, resp);
        return resp;
    }
    return Response(404, "Not Found");
} 