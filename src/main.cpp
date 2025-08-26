#include <sys/socket.h>
#include <netinet/in.h>   
#include <arpa/inet.h>    
#include <unistd.h>       
#include <iostream>
#include "../include/net/server.hpp"
#include "../include/http/router.hpp"

void index_handler(const Request& req, Response& resp) {
    resp.set_body("Hello, world!");
}

void html_page_handler(const Request& req, Response& resp) {
    std::string html = 
        "<!DOCTYPE html>"
        "<html lang=\"en\">"
        "<head>"
            "<meta charset=\"UTF-8\">"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
            "<title>Demo HTML Page</title>"
            "<style>"
                "body { font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }"
                ".container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
                "h1 { color: #333; text-align: center; }"
                ".feature { margin: 20px 0; padding: 15px; background: #f8f9fa; border-left: 4px solid #007bff; }"
                ".code { background: #e9ecef; padding: 10px; border-radius: 5px; font-family: monospace; }"
            "</style>"
        "</head>"
        "<body>"
            "<div class=\"container\">"
                "<h1>HTTP Server Demo</h1>"
                "<p>This is a demo HTML page served by your custom HTTP server!</p>"
                
                "<div class=\"feature\">"
                    "<h3>Features</h3>"
                    "<ul>"
                        "<li>Custom HTTP server implementation</li>"
                        "<li>Route handling with different content types</li>"
                        "<li>HTML, JSON, and plain text responses</li>"
                        "<li>Clean and modern design</li>"
                    "</ul>"
                "</div>"
                
                "<div class=\"feature\">"
                    "<h3>Available Routes</h3>"
                    "<div class=\"code\">"
                        "GET / - Plain text response<br>"
                        "GET /hello - Simple HTML<br>"
                        "GET /html - This page<br>"
                        "GET /api/users - JSON API response<br>"
                        "GET /api/status - JSON status endpoint"
                    "</div>"
                "</div>"
                
                "<div class=\"feature\">"
                    "<h3>Next Steps</h3>"
                    "<p>Try visiting the JSON endpoints to see different response types!</p>"
                "</div>"
            "</div>"
        "</body>"
        "</html>";
    
    resp.set_header("Content-Type", "text/html");
    resp.set_body(html);
}

void json_users_handler(const Request& req, Response& resp) {
    std::string json = 
        "{"
        "  \"users\": ["
        "    {"
        "      \"id\": 1,"
        "      \"name\": \"John Doe\","
        "      \"email\": \"john@example.com\","
        "      \"role\": \"admin\""
        "    },"
        "    {"
        "      \"id\": 2,"
        "      \"name\": \"Jane Smith\","
        "      \"email\": \"jane@example.com\","
        "      \"role\": \"user\""
        "    },"
        "    {"
        "      \"id\": 3,"
        "      \"name\": \"Bob Johnson\","
        "      \"email\": \"bob@example.com\","
        "      \"role\": \"user\""
        "    }"
        "  ],"
        "  \"total\": 3,"
        "  \"page\": 1,"
        "  \"per_page\": 10"
        "}";
    
    resp.set_header("Content-Type", "application/json");
    resp.set_body(json);
}

void json_status_handler(const Request& req, Response& resp) {
    std::string json = 
        "{"
        "  \"status\": \"ok\","
        "  \"timestamp\": \"2024-01-01T12:00:00Z\","
        "  \"server\": \"Custom HTTP Server\","
        "  \"version\": \"1.0.0\","
        "  \"uptime\": \"running\","
        "  \"endpoints\": ["
        "    \"/\","
        "    \"/hello\","
        "    \"/html\","
        "    \"/api/users\","
        "    \"/api/status\""
        "  ]"
        "}";
    
    resp.set_header("Content-Type", "application/json");
    resp.set_body(json);
}

int main() {

    // create router and add few demo routes
    Router router;
    router.add_route("GET", "/", index_handler);
    router.add_route("GET", "/html", html_page_handler);
    router.add_route("GET", "/api/users", json_users_handler);
    router.add_route("GET", "/api/status", json_status_handler);

    Server server(8080, router);
    server.start();

    return 0;
}



