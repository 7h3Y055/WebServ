#pragma once

#include "webserv.hpp"

class Response
{
    private:
        Request _req;
        int _status_code;
        std::string _status_message;
        std::map<std::string, std::string> _headers;
        std::vector<char> _body;
    public:

        std::vector<char> &get_response();
        void set_status_code(int status_code);
        void set_status_message(std::string status_message);
        void set_header(std::string key, std::string value);
        void set_body(std::vector<char> &body);

        Response(Request &req);
        ~Response();
};


Response *createResponse(int code, Request *req);
std::string get_error_message(int code);