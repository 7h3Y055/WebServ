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
    case 204:
        return "No Content";
        break;
    case 401:
        return "Unauthorized";
        break;
    case 408:
        return "Request Timeout";
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
    case 408:
        return "Request Timeout";
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
    case 301:
        return "Moved Permanently";
        break;
    case 302:
        return "Found";
        break;
    case 303:
        return "See Other";
        break;
    case 304:
        return "Not Modified";
        break;
    case 307:
        return "Temporary Redirect";
        break;
    case 308:
        return "Permanent Redirect";
        break;
    default:
        return "";
        break;
    }
}

std::string get_error_path(int code, int index){
    if (index < 0)
        index = 0;
    string key;
    if (!(code < 999 && code > 0))
        throw 500;
    string str;
    stringstream ss;
    ss << code;
    if (ss.fail())
        throw 400;
    ss >> key;
    return servers[index].getErrorPages()[key];
}

std::vector<char> get_error_body(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return vector<char>();
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file content into a vector
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return vector<char>();
    }

    return buffer;
}

Response *createResponse(int code, Request *req){
    Response *res = new Response(*req);
    res->set_status_code(code);
    res->set_status_message(get_error_message(code));
    res->set_header("Content-Type", "text/html");
    
    std::vector<char> body = get_error_body(get_error_path(code, req->get_server_index()));
    res->set_body(body);
    return res;
}