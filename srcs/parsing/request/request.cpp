#include "webserv.hpp"

std::vector<std::string> split_string_with_multiple_delemetres(std::string &str, std::string delimiters)
{
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
    if (header.find(':') == std::string::npos && strs.size() > 1)
        throw std::runtime_error("Bad Request2");
    if (std::isspace(header[header.find(':') - 1]))
        throw std::runtime_error("Bad Request3");
    pair.first = header.substr(0, header.find(':'));
    pair.second = header.substr(header.find(':') + 1, std::string::npos);
    pair.second = pair.second.substr(0, pair.second.find('\r'));
    if (std::isspace(pair.first[0])){
        std::cout << header << std::endl;
        throw std::runtime_error("Bad Request4");
    }
    return pair;
}


std::string &check_allowed_methods(std::string &method){
    return method;//HERE
}

void    Request::fill_request(std::string request){
    _Buffer += request;

    if (_Buffer.find("\r\n") == std::string::npos)
        return ;

    std::string line;
    while (_Buffer.find("\r\n") != std::string::npos)
    {
        line = _Buffer.substr(0, _Buffer.find("\r\n"));
        _Buffer = _Buffer.substr(_Buffer.find("\r\n") + 2, std::string::npos);
        if (line.empty() && _request_state == HTTP_REQUEST_LINE)
            return ;
        if (_request_state == HTTP_REQUEST_LINE)
        {
            std::vector<std::string> strs = split_string_with_multiple_delemetres(line, " \t\r\n");
            if (strs.size() == 3){
                _Method = check_allowed_methods(strs[0]);
                _URI = (strs[1][0] == '/' ? strs[1] : throw std::runtime_error("Invalid request"));
                _Version = (strs[2] == "HTTP/1.1" ? strs[2] : throw std::runtime_error("Invalid request"));
                _File_name = _URI.substr(0, _URI.find('?'));
            }
            else{
                std::cout << strs.size() << std::endl;
                throw std::runtime_error("Invalid request9");
            }
            _request_state = HTTP_HEADER;
        }
        else if (_request_state == HTTP_HEADER)
        {
            if (line.empty() || line == "\r"){
                if (_Host_found != 1){
                    throw std::runtime_error("Bad Request7");
                }
                _request_state = HTTP_BODY;
                continue ;
            }
            std::pair<std::string, std::string> header = fill_header(line);
            _Headers[header.first] = header.second;
            if (header.first == "Host"){
                _Host_found++;
                if (header.second.find_first_not_of("\r\n\v\t ") == std::string::npos)
                    throw std::runtime_error("Bad Request5");
            }
        }
        else if (_request_state == HTTP_BODY)
        {
            _Body += line += "\r\n";
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
