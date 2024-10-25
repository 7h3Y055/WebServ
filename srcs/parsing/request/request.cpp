#include "webserv.hpp"



std::vector<std::string> split_string_with_multiple_delemetres(std::string &str, std::string delimiters){
    std::vector<std::string> strs;
    size_t start_pos = 0;
    size_t end_pos = 0;

    while (end_pos != std::string::npos && start_pos < str.size())
    {
        end_pos = str.find_first_of(delimiters, start_pos);
        if (start_pos != end_pos)
            strs.push_back(str.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos + 1;
    }
    return strs;
}


std::pair<std::string, std::string> fill_header(std::string header){
    std::pair<std::string, std::string> pair;

    std::vector<std::string> strs = split_string_with_multiple_delemetres(header, ": \t\r");
    if (header.find(':') == std::string::npos && strs.size() > 1){
        throw std::runtime_error("Bad Request2");
    }

    if (std::isspace(header[header.find(':') - 1])){
        throw std::runtime_error("Bad Request3");
    }


    pair.first = header.substr(0, header.find(':'));
    pair.second = header.substr(header.find(':') + 1, std::string::npos);
    pair.second = pair.second.substr(0, pair.second.find('\r'));
    // if (std::isspace(pair.first[0])){
    //     throw std::runtime_error("Bad Request4");
    // }
    return pair;
}


void    Request::fill_request(std::string request){
    _Buffer += request;

    if (_Buffer.find("\r\n\r\n") == std::string::npos){
        return ;
    }
    if (_is_request_complete){
        _Body += request;
        return ;
    }


    std::stringstream ss(_Buffer);
    std::string line1;
    std::string line2;
    

    // First line
    while (std::getline(ss, line1)){
        if (line1 == "\r"){
            continue;
        }
        std::vector<std::string> strs = split_string_with_multiple_delemetres(line1, " \t");
        if (strs.size() == 3){
            _Method = strs[0];
            _URI = strs[1];
            _Version = strs[2];
            _File_name = _URI.substr(0, _URI.find('?'));
            break;
        }
        else{
            throw std::runtime_error("Invalid request1");
        }
    }


    // Headers
    std::pair<std::string, std::string>  header;
    while (std::getline(ss, line1))
    {
        header = fill_header(line1);
        _Headers[header.first] = header.second;
        std::getline(ss, line2);
        if (line1[line1.size() - 1] == '\r' && line2[0] == '\r')
            break;
        header = fill_header(line2);
        _Headers[header.first] = header.second;
    }
    
    // Body
    while (std::getline(ss, line1))
    {
        _Body += line1;
        _Body += '\n';
    }

    _is_request_complete = true;
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

std::string Request::get_version(){
    return this->_Version;
}

std::string Request::get_file_name(){
    return this->_File_name;
}

std::string Request::get_URI(){
    return this->_URI;
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



Request::Request(): _is_request_complete(false), _is_request_CGI(false), _already_filled(false){
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
