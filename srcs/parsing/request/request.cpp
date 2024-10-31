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

void remove_white_spaces_edges(std::string &str){
    str = str.substr(str.find_first_not_of(" \t\r\n\v"), std::string::npos);
    str = str.substr(0, str.find_last_not_of(" \t\r\n\v") + 1);
}

std::pair<std::string, std::string> fill_header(std::string header){
    std::pair<std::string, std::string> pair;
    std::vector<std::string> strs = split_string_with_multiple_delemetres(header, ": \t\r");
    if (header.find(':') == std::string::npos && strs.size() > 1)
        throw std::runtime_error("Bad Request2");
    if (std::isspace(header[header.find(':') - 1]))
        throw std::runtime_error("Bad Request3");
    pair.first = header.substr(0, header.find(':'));
    pair.second = header.substr(header.find(':') + 1, std::string::npos);
    pair.second = pair.second.substr(0, pair.second.find('\r'));
    remove_white_spaces_edges(pair.second);
    if (std::isspace(pair.first[0])){
        std::cout << header << std::endl;
        throw std::runtime_error("Bad Request4");
    }
    return pair;
}


std::string &check_allowed_methods(std::string &method){
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw std::runtime_error("Bad Request5");
    return method;
}

long long string2ll(std::string str){
    long long length;

    try{
        for (size_t i = 0; i < str.size(); i++){
            if (!std::isdigit(str[i]))
                throw std::runtime_error("Bad Request6");
        }
        length = std::atoll(str.c_str());
    }
    catch (std::exception &e){
        throw std::runtime_error("Bad Request7");
    }
    return length;
}

int ft_ishex(char c){
    if (std::isxdigit(c))
        return 1;
    return 0;
}

unsigned long long hex2ll(std::string str){
    str = str.substr(0, str.find('\r'));
    for (size_t i = 0; i < str.size(); ++i) {
        if (!ft_ishex(str[i])) {
            throw std::runtime_error("Bad Request8");
        }
    }

    std::stringstream ss;
    unsigned long long length;

    ss << std::hex << str;
    ss >> length;

    return length;
}

std::string check_URI(std::string URI){
    if (URI[0] != '/')
        throw std::runtime_error("Invalid request19");
    if (URI.size() > 2048)
        throw std::runtime_error("Invalid request20");
    if (URI.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::runtime_error("Invalid request21");
    return URI;
}

void    Request::fill_request(std::string request){
    _Buffer += request;
    if ( _Buffer.size() > 0 && _Transfer_Mechanism == "Chunked" && _request_state == HTTP_COMPLETE)
        throw std::runtime_error("Bad Request9");
    std::string line;
    static bool chunked_state = false;
    static unsigned long long chunked_length;
    bool have_new_line;
    while (_Buffer.size() > 0 && (_Buffer.find("\r\n") != std::string::npos || _request_state == HTTP_BODY))
    {
        if (_Buffer.find("\r\n") != std::string::npos)
            have_new_line = true;
        else
            have_new_line = false;
        line = _Buffer.substr(0, _Buffer.find("\r\n"));
        _Buffer = (_Buffer.find("\r\n") == std::string::npos) ? "" : _Buffer.substr(_Buffer.find("\r\n") + 2, std::string::npos);
        if (line.empty() && _request_state == HTTP_REQUEST_LINE)
            return ;
        if (_request_state == HTTP_REQUEST_LINE)
        {
            std::vector<std::string> strs = split_string_with_multiple_delemetres(line, " \t\r\n");
            if (strs.size() == 3){
                _Method = check_allowed_methods(strs[0]);
                _URI = check_URI(strs[1]);
                _Version = (strs[2] == "HTTP/1.1" ? strs[2] : throw std::runtime_error("Invalid request11"));
                _File_name = _URI.substr(0, _URI.find('?'));
            }
            else{
                throw std::runtime_error("Invalid request12");
            }
            _request_state = HTTP_HEADER;
        }
        else if (_request_state == HTTP_HEADER)
        {
            if (line.empty() || line == "\r"){
                if (_Host_found != 1){
                    throw std::runtime_error("Bad Request13");
                }
                if (_Method == "POST"){
                    if ((_Headers.find("Content-Length") != _Headers.end()) && (_Headers.find("Transfer-Encoding") != _Headers.end()))
                        throw std::runtime_error("Bad Request14");
                    if (_Headers.find("Content-Length") != _Headers.end() && !_Headers["Content-Length"].empty()){
                        _Transfer_Mechanism = "Fixed";
                        _Fixed_length = string2ll(_Headers["Content-Length"]);
                    }
                    else if (_Headers.find("Transfer-Encoding") != _Headers.end() && _Headers["Transfer-Encoding"] == "chunked")
                        _Transfer_Mechanism = "Chunked";
                    else
                        throw std::runtime_error("Bad Request15");
                }
                _request_state = HTTP_BODY;
                continue ;
            }
            std::pair<std::string, std::string> header = fill_header(line);
            _Headers[header.first] = header.second;
            if (header.first == "Host"){
                _Host_found++;
                if (header.second.find_first_not_of("\r\n\v\t ") == std::string::npos)
                    throw std::runtime_error("Bad Request16");
            }
        }
        else if (_request_state == HTTP_BODY)
        {
            if (_Transfer_Mechanism == "Fixed"){
                if (have_new_line){
                    line  += "\r\n";
                }
                if (_Body.size() + line.size() <= _Fixed_length){

                    _Body += line;
                }
                else
                {

                    _Body += line.substr(0, _Fixed_length - _Body.size());
                }
            }
            else if (_Transfer_Mechanism == "Chunked"){
                if (line == "0\r\n" && _Buffer == "\r\n"){
                    std::cout << "Done" << std::endl;
                    _request_state = HTTP_COMPLETE;
                    continue ;
                }
                if (chunked_state == false){
                    chunked_length = hex2ll(line);
                    chunked_state = true;
                }
                else{
                    if (line.size() < chunked_length)
                        throw std::runtime_error("Bad Request17");
                    chunked_state = false;
                    _Body += line.substr(0, chunked_length);
                }
            }
            if (_Body.size() >= 10){ // HERE
                throw std::runtime_error("Too Large");
            }
        }
    }
}



int Request::request_state(){
    return this->_request_state;
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

std::string Request::get_Host(){
    return this->_Headers["Host"];
}

std::string Request::get_transfer_mechanism(){
    return this->_Transfer_Mechanism;
}

long long Request::get_fixed_length(){
    return this->_Fixed_length;
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

void        Request::request_complete(){
    if (_Transfer_Mechanism == "Fixed"){
        _Body += _Buffer.substr(0, _Fixed_length - _Body.size());
        if (_Fixed_length != _Body.size()){
            throw std::runtime_error("Bad Request18");
        }
        _request_state = HTTP_COMPLETE;
    }
}




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //



Request::Request(): _request_state(HTTP_REQUEST_LINE), _is_request_CGI(false), _Host_found(false){
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
