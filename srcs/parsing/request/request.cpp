#include "webserv.hpp"








void    Request::fill_request(std::string request){
    _Buffer += request;

    if (_Buffer.find("\r\n\r\n") != std::string::npos){
        _is_request_complete = true;
    }

}

bool Request::is_request_complete(){
    return this->_is_request_complete;
}

bool Request::is_request_CGI(){
    return this->_is_request_CGI;
}

std::string Request::get_method(){
    return this->_Method;
}

std::string Request::get_file_name(){
    return this->_File_name;
}

std::string Request::get_URI(){
    return this->_URI;
}

std::string Request::get_host(){
    return this->_Host;
}

std::map<std::string, std::string> Request::get_headers(){
    return this->_Headers;
}

std::string Request::get_header(std::string key){
    return this->_Headers[key];
}

std::string Request::get_body(){
    return this->_Body;
}





// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //



Request::Request(): _is_request_complete(false), _is_request_CGI(false){
}

Request::~Request(){

}

Request::Request(const Request& other){

}

Request& Request::operator=(const Request& other){
    if (this != &other)
    {

    }
    return *this;
}

Request::Request(Request& other){

}

Request& Request::operator=(Request& other){
    if (this != &other){

    }
    return *this;
}
