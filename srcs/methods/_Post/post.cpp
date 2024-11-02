#include "webserv.hpp"

Response &post_Response(Request &req){
    Response *res = new Response(req);
    res->set_status_code(200);
    res->set_status_message("OK");
    res->set_header("Content-Type", "text/html");
    std::string content = "<html><body><h1>POST</h1></body></html>";
    std::vector<char> body(content.begin(), content.end());
    res->set_body(body);
    return *res;
}

