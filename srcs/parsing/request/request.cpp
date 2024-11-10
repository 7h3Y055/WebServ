#include "webserv.hpp"



std::vector<std::string> split_string_with_multiple_delemetres(std::string &str, std::string delimiters){
    std::vector<std::string> strs;
    size_t start_pos = 0;
    size_t end_pos = 0;

    if (str.empty())
        return strs;
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
        throw 400;
    if (std::isspace(header[header.find(':') - 1]))
        throw 400;
    pair.first = header.substr(0, header.find(':'));
    pair.second = header.substr(header.find(':') + 1, std::string::npos);
    pair.second = pair.second.substr(0, pair.second.find('\r'));
    remove_white_spaces_edges(pair.second);
    if (std::isspace(pair.first[0])){
        std::cout << header << std::endl;
        throw 400;
    }
    return pair;
}

long long string2ll(std::string str){
    unsigned long long length;

    try{
        for (size_t i = 0; i < str.size(); i++){
            if (!std::isdigit(str[i]))
                throw 400;
        }
        std::stringstream ss;
        ss << str;
        ss >> length;
        if (ss.fail()) {
            throw 400;
        }
    }
    catch (...){
        throw 400;
    }
    return length;
}

int ft_ishex(char c){
    if (std::isxdigit(c))
        return 1;
    return 0;
}

unsigned long long hex2ll(std::string str){
    unsigned long long length;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!ft_ishex(str[i])) {
        cout << "111111111: " << string(str.begin(), str.begin() + 7) << endl;
            throw 400;
        }
    }
    std::stringstream ss;
    ss << std::hex << str;
    ss >> length;

    if (ss.fail()){
        cout << "111111111: " << endl;
        throw 400; // HERE 
    }

    return length;
}

std::string check_URI(std::string URI){
    if (URI[0] != '/')
        throw 400;
    if (URI.size() > 2048)
        throw 414;
    if (URI.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw 400;
    return URI;
}

bool buffer_have_nl(std::vector<char> &buf, int _request_state, std::string _Transfer_Mechanism){
    if (_request_state == HTTP_BODY && _Transfer_Mechanism == "Fixed")
        return true;
    for (size_t i = 0; i < buf.size(); i++){
        if (buf[i] == '\r' && buf[i + 1] == '\n')
            return true;
    }
    return false;
}

bool buffer_have_nl(std::vector<char> &buf){
    for (size_t i = 0; i < buf.size(); i++){
        if (buf[i] == '\r' && buf[i + 1] == '\n')
            return true;
    }
    return false;
}

std::string get_http_line(std::vector<char> *buf){
    // size_t pos = string(buf->begin(), buf->end()).find("\r\n");
    // string s = string(buf->begin(), buf->end()).substr(0, pos);
    // return s;

    std::string line;
    for (size_t i = 0; i < buf->size() - 1; i++){
        if ((*buf)[i] == '\r' && (*buf)[i + 1] == '\n'){
            line = std::string(buf->begin(), buf->begin() + i);
            buf->erase(buf->begin(), buf->begin() + i + 2);
            return line;
        }
    }
    return line;
}

bool is_CGI(std::string file_name){
    if (false)
        return true;
    return false;
}

void    Request::fill_request(std::vector<char> &buf){
    _Buffer.insert(_Buffer.end(), buf.begin(), buf.end());
    std::string line;
    static bool chunked_state = false;
    static unsigned long long chunked_length;
    
    while (_Buffer.size() > 0 && buffer_have_nl(_Buffer, _request_state, _Transfer_Mechanism) && _request_state != HTTP_COMPLETE)
    {
        if (_request_state == HTTP_REQUEST_LINE){
            line = get_http_line(&_Buffer);
            if (line.empty())
                continue;
            std::vector<std::string> strs = split_string_with_multiple_delemetres(line, " \t\r\n");
            if (strs.size() == 3){
                _Method = strs[0];
                _URI = check_URI(strs[1]);
                _Version = (strs[2] == "HTTP/1.1" ? strs[2] : throw 505);
                _File_name = _URI.substr(0, _URI.find('?'));
                _is_request_CGI = is_CGI(_File_name);
            }
            else{
                throw 400;
            }
            _request_state = HTTP_HEADER;
        }
        if (_request_state == HTTP_HEADER){
            line = get_http_line(&_Buffer);
            line = line.substr(0, line.find("\r\n"));
            if (line.empty()){
                if (_Host_found != 1){
                    throw 400;
                }
                if (_Method == "POST"){
                    if ((_Headers.find("Content-Length") != _Headers.end()) && (_Headers.find("Transfer-Encoding") != _Headers.end()))
                        throw 400;
                    if (_Headers.find("Content-Length") != _Headers.end() && !_Headers["Content-Length"].empty()){
                        _Transfer_Mechanism = "Fixed";
                        _Fixed_length = string2ll(_Headers["Content-Length"]);
                    }
                    else if (_Headers.find("Transfer-Encoding") != _Headers.end()){
                        if (_Headers["Transfer-Encoding"] == "chunked")
                            _Transfer_Mechanism = "Chunked";
                        else
                            throw 501;
                    }
                    else
                        throw 400;
                }
                if (_Method != "POST" || (_Transfer_Mechanism == "Fixed" && _Fixed_length == 0)){
                    _request_state = HTTP_COMPLETE;
                }
                else{
                    _request_state = HTTP_BODY;
                }
                continue ;
            }
            std::pair<std::string, std::string> header = fill_header(line);
            _Headers[header.first] = header.second;
            if (header.first == "Host"){
                _Host_found++;
                if (header.second.find_first_not_of("\r\n\v\t ") == std::string::npos)// if the Host found but empty
                    throw 400;
            }
        }
        if (_request_state == HTTP_BODY){
            ofstream file;
            if (_Body_path.size() == 0){
                do{
                    file.close();
                    _Body_path = "/tmp/" + generate_random_name();
                    file.open(_Body_path.c_str());
                } while (file.is_open() == 0);
            }
            file.open(_Body_path.c_str(), std::ios::app);
                if (!file.is_open())
                    throw 500;
            if (_Transfer_Mechanism == "Fixed"){
                size_t size = (_Fixed_length < _Buffer.size() ? _Fixed_length : _Buffer.size());
                file.write(&(*_Buffer.begin()),  size);
                _Buffer.erase(_Buffer.begin(), _Buffer.begin() + size);
                std::streampos pos = file.tellp();
                if (pos == _Fixed_length){
                    _request_state = HTTP_COMPLETE;
                }
            }
            else if (_Transfer_Mechanism == "Chunked"){
                if (chunked_state == false){
                    if (_Buffer[0] == '\r' && _Buffer[1] == '\n')
                        _Buffer.erase(_Buffer.begin(), _Buffer.begin() + 2);
                    if (buffer_have_nl(_Buffer) == false){
                        return ;
                    }
                    std::string len = get_http_line(&_Buffer);
                    chunked_length = hex2ll(len);
                    chunked_state = true;
                    if (chunked_length == 0){
                        _request_state = HTTP_COMPLETE;
                        chunked_state = false;
                    }
                }
                if (chunked_state == true){
                    if (chunked_length > _Buffer.size())
                        return ;
                    file.write(&(*_Buffer.begin()), chunked_length);
                    file.flush();
                    _Buffer.erase(_Buffer.begin(), _Buffer.begin() + chunked_length);
                    chunked_state = false;
                }
            }
            if (file.tellp() > servers[server_index].getClientMaxBodySize()){
                file.close();
                throw 413;
            }
            file.close();

        }
    }
}

Response *Request::execute_request(){
    location loc = get_location(get_file_name(), servers[get_server_index()]);

    size_t i;
    for (i = 0; i < loc.getMethods().size(); i++)
        if (loc.getMethods()[i] == _Method)
            break;
    if (i == loc.getMethods().size())
        throw 405;
    

    if (is_request_CGI()){
        std::cout << "[CGI]" << std::endl;
        // return CGI_Response();
    }
    else if (_Method == "GET"){
        std::cout << "[GET]" << std::endl;
        return get_Response(*this);
    }
    else if (_Method == "POST"){
        std::cout << "[POST]" << std::endl;
        return post_Response();
    }
    else if (_Method == "DELETE"){
        std::cout << "[DELETE]" << std::endl;
        // return delete_Response();
    }

    return createResponse(404, this);
}


RequestState &Request::request_state(){
    return this->_request_state;
}

bool Request::is_request_CGI(){
    return this->_is_request_CGI;
}

std::string &Request::get_method(){
    return this->_Method;
}

std::string &Request::get_version(){
    return this->_Version;
}

std::string &Request::get_Host(){
    return this->_Headers["Host"];
}

std::string &Request::get_transfer_mechanism(){
    return this->_Transfer_Mechanism;
}

long long &Request::get_fixed_length(){
    return this->_Fixed_length;
}

std::string &Request::get_file_name(){
    return this->_File_name;
}

std::string &Request::get_URI(){
    return this->_URI;
}

std::map<std::string, std::string> &Request::get_headers(){
    return this->_Headers;
}

std::string &Request::get_header(std::string key){
    return this->_Headers[key];
}

std::string &Request::get_body_path(){
    return this->_Body_path;
}





// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //



Request::Request(): _request_state(HTTP_REQUEST_LINE), _is_request_CGI(false), _Host_found(false){
}

Request::~Request(){
}
