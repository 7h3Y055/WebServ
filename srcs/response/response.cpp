#include "Response.hpp"



std::vector<char> &Response::get_response(){
    std::vector<char> *response = new std::vector<char>;
    std::stringstream ss;
    std::string server_name = "webserv/29.16666666729";
    ss << _status_code;
    
    response->insert(response->end(), _req.get_version().begin(), _req.get_version().end());
    response->push_back(' ');
    response->insert(response->end(), ss.str().begin(), ss.str().end());
    response->push_back(' ');
    response->insert(response->end(), _status_message.begin(), _status_message.end());
    response->push_back('\r');response->push_back('\n');

    // add headers
    // response->insert(response->end(), "Server: ".begin(), "Server: ".end());
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
        response->insert(response->end(), it->first.begin(), it->first.end());
        response->push_back(':');
        response->push_back(' ');
        response->insert(response->end(), it->second.begin(), it->second.end());
        response->push_back('\r');response->push_back('\n');
    }

    // add extra headers


    response->push_back('\r');response->push_back('\n');

    // add body fixed or chunked
    response->insert(response->end(), _body.begin(), _body.end());
    
    return *response;
}

void Response::set_status_code(int status_code){
    this->_status_code = status_code;
}

void Response::set_status_message(std::string status_message){
    this->_status_message = status_message;
}

void Response::set_header(std::string key, std::string value){
    this->_headers[key] = value;
}

void Response::set_body(std::vector<char> &body){
    this->_body = body;
}



Response::Response(Request &req): _req(req)
{
}
Response::~Response()
{
}
