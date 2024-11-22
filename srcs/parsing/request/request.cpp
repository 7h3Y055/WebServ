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
        if (start_pos != end_pos && start_pos != string::npos)
            strs.push_back(str.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos + 1;
    }
    return strs;
}

void remove_white_spaces_edges(std::string &str){
    size_t start = str.find_first_not_of(" \t\r\n\v");
    if (start == std::string::npos){
        str = "";
        return ;
    }
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
    if (header.find(':') == std::string::npos)
        throw 400;
    pair.second = header.substr(header.find(':') + 1, std::string::npos);
    pair.second = pair.second.substr(0, pair.second.find('\r'));
    remove_white_spaces_edges(pair.second);
    if (std::isspace(pair.first[0])){
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

unsigned long long hex2ll(std::string &str){
    unsigned long long length;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!ft_ishex(str[i])) {
            throw 400;
        }
    }
    std::stringstream ss;
    ss << std::hex << str;
    ss >> length;

    if (ss.fail()){
        throw 400;
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

bool is_CGI(std::string file_name, size_t index, size_t start_pos){
    location loc = get_location(file_name, servers[index]);
    if (loc.getCgi().size() == 0 || start_pos >= file_name.size())
        return false;

    size_t i = start_pos;

    for (; file_name[i] == '/'; i++);
    string file = file_name.substr(i);
    size_t pos2 = file.find_first_of('/', start_pos);

    size_t pos = file.find_last_of('.', pos2);
    if (pos != std::string::npos){
        string extention = file.substr(pos, pos2 - pos);
        if (loc.getCgi()[extention].size() != 0){
            return true;
        }
        else if (pos2 != std::string::npos){
            return is_CGI(file_name, index, pos2 + 1);
        }
    }
    else if (pos2 <= file_name.size()){
        return is_CGI(file_name, index, pos2 + 1);
    }
    return false;
}

string get_CGI_script(std::string file_name, size_t index, size_t start_pos){
    if (file_name.find(".") == string::npos)
        return file_name;
    location loc = get_location(file_name, servers[index]);

    size_t i = start_pos;

    for (; file_name[i] == '/'; i++);
    string file = file_name;
    file = file.substr(i);
    size_t pos2 = file.find_first_of('/', start_pos);

    size_t pos = file.find_last_of('.', pos2);
    if (pos != std::string::npos){
        string extention = file.substr(pos, pos2 - pos);
        if (loc.getCgi()[extention].size() != 0){
            return file_name.substr(0, file_name.find(extention) + extention.size());
        }
        else if (pos2 != std::string::npos){
            return get_CGI_script(file_name, index, pos2);
        }
    }
    else if (pos2 <= file_name.size()){
        return get_CGI_script(file_name, index, pos2);
    }
    return file_name;
}

void    Request::fill_request(std::vector<char> &buf){
    _Buffer.insert(_Buffer.end(), buf.begin(), buf.end());
    std::string line;

    while (_Buffer.size() > 0 && buffer_have_nl(_Buffer, _request_state, _Transfer_Mechanism) && _request_state != HTTP_COMPLETE)
    {
        if (_request_state == HTTP_REQUEST_LINE){
            line = get_http_line(&_Buffer);
            if (line.empty())
                continue;
            std::vector<std::string> strs = split_string_with_multiple_delemetres(line, " \t\r\n");
            if (_Method != "POST" || (_Transfer_Mechanism == "Fixed" && _Fixed_length == 0)){
            {
                _Body_path =  generate_random_name();
                _request_state = HTTP_COMPLETE;

            }
            }
            if (strs.size() == 3){
                _Method = strs[0];
                _URI = check_URI(strs[1]);
                _Version = (strs[2] == "HTTP/1.1" ? strs[2] : throw 505);
                _File_name = _URI.substr(0, _URI.find('?'));
                if (_File_name.find("..") != string::npos)
                    throw 403;
            }
            else{
                throw 400;
            }
            _request_state = HTTP_HEADER;
        }
        if (_request_state == HTTP_HEADER){
            if (_Buffer.empty() && _Host_found != 1)
                throw 400;
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
                    cout << "Server index: " << get_server_index_(get_Host(), _fd) << endl;
                    set_server_index(get_server_index_(get_Host(), _fd));
                    _request_state = HTTP_COMPLETE;
                }
                else{
                    set_server_index(get_server_index_(get_Host(), _fd));
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
                _Body_path =  generate_random_name();
                cout << "Create: " << _Body_path << endl;
            }
            file.open(_Body_path.c_str(), std::ios::app);
            if (!file.is_open())
                throw 500;
            if (_Transfer_Mechanism == "Fixed"){
                size_t size = (static_cast<size_t>(_Fixed_length) < _Buffer.size() ? _Fixed_length : _Buffer.size());
                file.write(&(*_Buffer.begin()), size);
                if (file.fail())
                    throw 500;
                _Buffer.erase(_Buffer.begin(), _Buffer.begin() + size);
                if (file.tellp() == _Fixed_length){
                    _request_state = HTTP_COMPLETE;
                } else if (file.tellp() > _Fixed_length)
                    throw 413;
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
                    if (chunked_length > 65536)
                        throw 413;
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

Response *create_redirection(location &loc, Request &req){
    Response *res = new Response(req);

    int code = loc.getRedirection().begin()->first;
    res->set_status_code(code);
    res->set_status_message(get_error_message(code));
    if (code == 301 || code == 302 || code == 303 || code == 304 || code == 307  || code == 308){
        res->set_header("Location", loc.getRedirection().begin()->second);
    }
    else{
        res->set_header("Content-Type", "application/octet-stream");
        vector<char> body(loc.getRedirection().begin()->second.begin(), loc.getRedirection().begin()->second.end());
        res->set_body(body);
    }



    return res;
}

Response *Request::execute_request()
{
    location loc = get_location(get_file_name(), servers[get_server_index()]);

    if (loc.getRedirection().size() == 1){
        cout << "[Redirection]" << endl;
        return create_redirection(loc, *this);
    }
    size_t i;
    for (i = 0; i < loc.getMethods().size(); i++)
        if (loc.getMethods()[i] == _Method)
            break;
    if (i == loc.getMethods().size())
        throw 405;
    


    if (_Method == "POST"){
        std::cout << "[POST]" << std::endl;
        return post_Response();
    }
    else if (_Method == "DELETE"){
        std::cout << "[DELETE]" << std::endl;
        delete_Response(this);
        throw 204;
    }

    return createResponse(404, this);
}


RequestState &Request::request_state(){
    return this->_request_state;
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


Request::Request(): _request_state(HTTP_REQUEST_LINE), _Host_found(false), _Fixed_length(0), chunked_state(false), chunked_length(0){
}

Request::~Request(){
}
