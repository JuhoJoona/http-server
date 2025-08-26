#include "../../include/http/router.hpp"

Router::Router() {
    // Initialize routes map
}

Router::~Router() {
    // Cleanup if needed
}

void Router::handle_request(Request& request, Response& response) {
    // Default response for now
    response = Response(404, "Not Found", "Route not found");
} 