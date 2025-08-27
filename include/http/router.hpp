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
    Router();
    ~Router();
    
    void add_route(const std::string& method, const std::string& path, Handler handler);
    Response route(const Request& req) const;

private:
    std::unordered_map<std::string, Handler> routes_;
};
