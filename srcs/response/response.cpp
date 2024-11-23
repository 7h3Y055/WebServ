#include "Response.hpp"

void push_str(std::vector<char> &vec, std::string str){
    vec.insert(vec.end(), str.begin(), str.end());
}

std::string get_date_header() {
    time_t t;
    time(&t);

    std::string d = ctime(&t);

    std::string date = "Date: ";

    date += d.substr(0,3);
    date += ", ";
    date += d.substr(8, 2);
    date += " ";
    date += d.substr(4, 3);
    date += " ";
    date += d.substr(20, 4);
    date += " ";
    date += d.substr(11, 8);
    date += " GMT";

    return date;
}

std::vector<char> Response::get_response(){
    std::vector<char> response;
    std::stringstream ss;
    std::string server_name = "Server: webserv/29.16666666729";
    std::string str;
    ss << _status_code;
    

    // add status line
    push_str(response, "HTTP/1.1 " + ss.str() + " " + _status_message + "\r\n");

    // add headers
    push_str(response, server_name + "\r\n");
    push_str(response, get_date_header() + "\r\n");
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
        push_str(response, it->first + ": " + it->second + "\r\n");
    }

    // add extra headers

    ss.str("");
    ss << _body.size();
    push_str(response,  "Content-Length: " + ss.str() + "\r\n\r\n");

    response.insert(response.end(), _body.begin(), _body.end());
    push_str(response, "\r\n");

    return response;
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

std::string &Response::body_file_path_ref(){
    return this->body_file_path;
}


Response::Response(Request &req): _req(req)
{
}

Response::~Response()
{
}
