#include "webserv.hpp"

std::string get_error_message(int code){
    switch (code)
    {
    case 400:
        return "Bad Request";
        break;
    case 201:
        return "Created";
        break;
    case 401:
        return "Unauthorized";
        break;
    case 403:
        return "Forbidden";
        break;
    case 404:
        return "Not Found";
        break;
    case 405:
        return "Method Not Allowed";
        break;
    case 413:
        return "Request Entity Too Large";
        break;
    case 500:
        return "Internal Server Error";
        break;
    case 501:
        return "Not Implemented";
        break;
    case 505:
        return "HTTP Version Not Supported";
        break;
    default:
        return "Unknown Error";
        break;
    }
}

std::string get_error_path(int code){
    return "error_pages/404.html";
}

std::vector<char> get_error_body(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file content into a vector
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    return buffer;
}

Response *createResponse(int code, Request *req){
    Response *res = new Response(*req);
    res->set_status_code(code);
    res->set_status_message(get_error_message(code));
    res->set_header("Content-Type", "text/html");
    std::vector<char> body = get_error_body(get_error_path(code));
    // body.shrink_to_fit();
    res->set_body(body);
    return res;
}