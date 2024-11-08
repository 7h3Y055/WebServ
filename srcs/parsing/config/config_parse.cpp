#include "webserv.hpp"

std::string trim(std::string &str)
{
    size_t first = str.find_first_not_of("\t\n\v\f\r ");
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of("\t\n\v\f\r ");
    str = str.substr(first, (last - first + 1));
    return str;
}

bool just_spaces(const std::string &str){
    return str.find_first_not_of("\t\n\v\f\r ") == std::string::npos;
}

void parse_methods(std::vector<string> &methods, std::string &str)
{
    if (str.size() == 0)
        methods.clear();
    else{
        std::vector<string> strs = split_string_with_multiple_delemetres(str, "\t\n\v\f\r ,");
        for (size_t i = 0; i < strs.size(); ++i){
            if (strs[i] != "GET" && strs[i] != "POST" && strs[i] != "DELETE")
                throw std::runtime_error("Error: invalid method");
            for (std::vector<string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
                if (*it == strs[i])
                    throw std::runtime_error("Error: duplicate method");
            }
            methods.push_back(strs[i]);
        }
    }
}

void init_location(location &loc, std::vector<string> &strs)
{
    if (strs.size() > 2)
        for (size_t i = 2; i < strs.size(); ++i)
            strs[1] += "=" + strs[i];


    trim(strs[0]);
    if (strs[0] == "methods" || strs[0] == "METHODS"){
        if (loc.already_set["methods"] == "true")
            throw std::runtime_error("Error: duplicate methods");
        loc.already_set["methods"] = "true";
        if (strs.size() == 2){
            parse_methods(loc.getMethods(), strs[1]);
        }
    }
    else if (strs[0] == "index" || strs[0] == "INDEX"){
        if (loc.already_set["index"] == "true")
            throw std::runtime_error("Error: duplicate index");
        loc.already_set["index"] = "true";
        if (strs.size() < 2)
            throw std::runtime_error("Error: invalid index");
        loc.setIndex(split_string_with_multiple_delemetres(strs[1], "\t\n\v\f\r ,"));
    }
    else if (strs[0] == "directory_listing" || strs[0] == "DIRECTORY_LISTING"){
        if (loc.already_set["directory_listing"] == "true")
            throw std::runtime_error("Error: duplicate directory_listing");
        loc.already_set["directory_listing"] = "true";
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid directory_listing");
        if (strs[1] == "on" || strs[1] == "ON")
            loc.setDirectoryListing(true);
        else if (strs[1] == "off" || strs[1] == "OFF")
            loc.setDirectoryListing(false);
        else
            throw std::runtime_error("Error: invalid directory_listing");
    }
    else if (strs[0] == "upload_path" || strs[0] == "UPLOAD_PATH"){
        if (loc.already_set["upload_path"] == "true")
            throw std::runtime_error("Error: duplicate upload_path");
        loc.already_set["upload_path"] = "true";
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid upload_path");
        loc.setUploadPath(strs[1]);
    }
    else if (strs[0] == "cgi" || strs[0] == "CGI"){
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid cgi");
        std::vector<string> cgi = split_string_with_multiple_delemetres(strs[1], "\t\n\v\f\r :");
        if (cgi.size() != 2)
            throw std::runtime_error("Error: invalid cgi");
        std::map<std::string, std::string> cgi_map;
        // cgi_map[cgi[0]] = cgi[1];
        loc.getCgi()[cgi[0]] = cgi[1];
    }
    else if (strs[0] == "redirection" || strs[0] == "REDIRECTION"){
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid redirection");
        if (loc.already_set["redirection"] == "true")
            throw std::runtime_error("Error: duplicate redirection");
        loc.already_set["redirection"] = "true";
        std::vector<string> redirection = split_string_with_multiple_delemetres(strs[1], "\t\n\v\f\r ");
        if (redirection.size() != 2 || redirection[0].size() != 3)
            throw std::runtime_error("Error: invalid redirection");
        for (size_t i = 0; i < redirection[0].size(); ++i){
            if (!isdigit(redirection[0][i]))
                throw std::runtime_error("Error: invalid redirection");
        }
        loc.getRedirection()[std::atoi(redirection[0].c_str())] =  redirection[1];
    }
    else if (strs[0] == "root" || strs[0] == "ROOT"){
        if (loc.already_set["root"] == "true")
            throw std::runtime_error("Error: duplicate root");
        loc.already_set["root"] = "true";
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid root");
        loc.setRoot(strs[1]);
    }
    else{
        throw std::runtime_error("Error: invalid location");
    }
}

void read_location(std::ifstream &file, size_t &line_n, location &loc)
{
    std::string line;
    std::vector<string> strs;
    while (std::getline(file, line))
    {
        trim(line);
        if (just_spaces(line) || line[0] == '#')
            continue;
        if (line == "}")
            return;

        strs = split_string_with_multiple_delemetres(line, "=");
        init_location(loc, strs);
        // std::cout << strs[0] << std::endl;
        
        line_n++;
    }
}

double convert_to_byte(std::string str){
    char last = str[str.size() - 1];
    if (std::isdigit(last))
        last = 0;
    else
        str = str.substr(0, str.size() - 1);
    for (size_t i = 0; i < str.size(); i++)
        if (!isdigit(str[i]))
            throw std::runtime_error("Error: invalid client_max_body_size");
    double n = atof(str.c_str());
    if (n < 0)
        throw std::runtime_error("Error: invalid client_max_body_size");
    if (last == 'k' || last == 'K')
        return n * 1024;
    else if (last == 'm' || last == 'M')
        return n * 1024 * 1024;
    else if (last == 'g' || last == 'G')
        return n * 1024 * 1024 * 1024;
    else
        return n;
    return 0;
}

void init_server(Serv &server, std::vector<string> &strs){
    if (strs.size() > 1)
        for (size_t i = 1; i < strs.size(); ++i)
            strs[0] += " " + strs[i];
    

    std::vector<string> server_strs = split_string_with_multiple_delemetres(strs[0], "=");
    if (server_strs.size() != 2)
        throw std::runtime_error("Error: invalid server3");
    trim(server_strs[0]);
    trim(server_strs[1]);

    if (server_strs[0] == "port" || server_strs[0] == "PORT"){
        if (server.already_set["port"] == "true")
            throw std::runtime_error("Error: duplicate port");
        server.already_set["port"] = "true";
        if (server_strs.size() < 2)
            throw std::runtime_error("Error: invalid port");
        for (size_t i = 0; i < server_strs[1].size(); ++i){
            if (!isdigit(server_strs[1][i]))
                throw std::runtime_error("Error: invalid port");
        }
        double n = atof(server_strs[1].c_str());
        if (n < 0 || n > 65535 || n != (int)n)
            throw std::runtime_error("Error: invalid port");
        server.getPort() = n;
    }
    else if (server_strs[0] == "host" || server_strs[0] == "HOST"){
        if (server.already_set["host"] == "true")
            throw std::runtime_error("Error: duplicate host");
        server.already_set["host"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid host");
        server.getHost() = server_strs[1];
    }
    else if (server_strs[0] == "server_names" || server_strs[0] == "SERVER_NAMEs"){
        if (server.already_set["server_names"] == "true")
            throw std::runtime_error("Error: duplicate server_names");
        server.already_set["server_names"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid server_names");
        std::vector<string> server_names = split_string_with_multiple_delemetres(server_strs[1], "\t\n\v\f\r ,");
        for (size_t i = 0; i < server_names.size(); ++i){
            server.getServerName().push_back(server_names[i]);
        }
    }
    else if (server_strs[0] == "root" || server_strs[0] == "ROOT"){
        if (server.already_set["root"] == "true")
            throw std::runtime_error("Error: duplicate root");
        server.already_set["root"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid root");
        server.getRoot() = server_strs[1];
    }
    else if (server_strs[0] == "client_max_body_size" || server_strs[0] == "CLIENT_MAX_BODY_SIZE"){
        if (server.already_set["client_max_body_size"] == "true")
            throw std::runtime_error("Error: duplicate client_max_body_size");
        server.already_set["client_max_body_size"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid client_max_body_size");
        server.setClienMaxBodySize(convert_to_byte(server_strs[1]));
    }
    else if (server_strs[0] == "error_page" || server_strs[0] == "ERROR_PAGE"){
        std::vector<string> error_page = split_string_with_multiple_delemetres(server_strs[1], "\t\n\v\f\r :");
        if (error_page.size() != 2)
            throw std::runtime_error("Error: invalid error_page");
        if (server.getErrorPages().find(error_page[0]) != server.getErrorPages().end())
            throw std::runtime_error("Error: duplicate error_page");
        server.getErrorPages()[error_page[0]] = error_page[1];
    }
    else{
        throw std::runtime_error("Error: invalid server4");
    }
}

void add_default_root(location &loc, std::string root){
    if (loc.getRoot() == "")
        loc.setRoot(root);
}

void read_server(std::ifstream &file, size_t &line_n, Serv &server)
{
    std::string line;
    std::vector<string> strs;
    while (std::getline(file, line))
    {
        trim(line);
        if (just_spaces(line) || line[0] == '#')
            continue;
        if (line == "}SERVER")
            return;
        strs = split_string_with_multiple_delemetres(line, "\t\n\v\f\r ");
        if (strs[0] == "location" || strs[0] == "LOCATION")
        {
            if (strs.size() != 3 || strs[2] != "{")
                throw std::runtime_error("Error: invalid location");
            location loc;
            loc.setPath(strs[1]);
            add_default_root(loc, server.getRoot());
            read_location(file, line_n, loc);
            server.getLocations().push_back(loc);
        }
        else{
            init_server(server, strs);
        }
        
        line_n++;
    }
    throw std::runtime_error("Error: invalid server1");
}

void set_default_error_pages(Serv &server){
    if (server.getErrorPages().find("400") == server.getErrorPages().end())
        server.getErrorPages()["400"] = "error_pages/400.html";
    if (server.getErrorPages().find("401") == server.getErrorPages().end())
        server.getErrorPages()["401"] = "error_pages/401.html";
    if (server.getErrorPages().find("403") == server.getErrorPages().end())
        server.getErrorPages()["403"] = "error_pages/403.html";
    if (server.getErrorPages().find("404") == server.getErrorPages().end())
        server.getErrorPages()["404"] = "error_pages/404.html";
    if (server.getErrorPages().find("405") == server.getErrorPages().end())
        server.getErrorPages()["405"] = "error_pages/405.html";
    if (server.getErrorPages().find("500") == server.getErrorPages().end())
        server.getErrorPages()["500"] = "error_pages/500.html";
    if (server.getErrorPages().find("501") == server.getErrorPages().end())
        server.getErrorPages()["501"] = "error_pages/501.html";
    if (server.getErrorPages().find("505") == server.getErrorPages().end())
        server.getErrorPages()["505"] = "error_pages/505.html";
    if (server.getErrorPages().find("414") == server.getErrorPages().end())
        server.getErrorPages()["414"] = "error_pages/414.html";
    if (server.getErrorPages().find("413") == server.getErrorPages().end())
        server.getErrorPages()["413"] = "error_pages/413.html";
    if (server.getErrorPages().find("409") == server.getErrorPages().end())
        server.getErrorPages()["409"] = "error_pages/413.html";
}

void add_default_location(Serv &server){
    for (size_t i = 0; i < server.getLocations().size(); ++i){
        if (server.getLocations()[i].getPath() == "/")
            return;
    }
    location loc;
    loc.getPath() = "/";
    loc.getMethods().push_back("GET");
    loc.setDirectoryListing(false);
    server.getLocations().push_back(loc);
}


void check_duplacate_locations(Serv &server){
    for (size_t i = 0; i < server.getLocations().size(); ++i) {
        for (size_t j = 0; j < server.getLocations().size(); ++j) {
            if (server.getLocations()[i].getPath() == server.getLocations()[j].getPath() && i != j)
                throw std::runtime_error("Error: duplicate location");
        }
    }
}

void parse_config(int ac, char **av)
{
    // std::vector<Serv> servers;
    std::string config_path;

    if (ac == 2)
        config_path = av[1];
    else if (ac == 1)
        config_path = "conf/default.conf";
    else {
        throw std::runtime_error("Usage: ./webserv <config_file>");
        
    }
    
    std::ifstream file(config_path.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: could not open file " + config_path);

    std::string line;
    size_t line_n = 0;
    int server_n = 1;
    while (std::getline(file, line))
    {
        trim(line);
        if (just_spaces(line) || line[0] == '#')
            continue;
        if (line == "SERVER{")
        {
            Serv server;
            read_server(file, line_n, server);
            set_default_error_pages(server);
            if (server.getPort() == 0 || server.getHost() == "" || server.getRoot() == "" || (server.getServerName().size() == 0 && server_n > 1))
                throw std::runtime_error("Error: invalid server2");
            add_default_location(server);
            check_duplacate_locations(server);
            servers.push_back(server);
            server_n++;
        }
        else if (!line.empty()){
            throw std::runtime_error("Error: invalid config file");
        }
        line_n++;
    }
    for (size_t i = 0; i < servers.size(); ++i) {
        for (size_t j = i + 1; j < servers.size(); ++j) {
            for (size_t k = 0; k < servers[i].getServerName().size(); ++k) {
                for (size_t l = 0; l < servers[j].getServerName().size(); ++l) {
                    if (servers[i].getServerName()[k] == servers[j].getServerName()[l])
                        throw std::runtime_error("Error: duplicate server name");
                }
            }
        }
    }
    if (servers.size() == 0)
        throw std::runtime_error("Error: no server");
    // return servers;
}


Serv::Serv():host(""), root(""), client_max_body_size(-1)
{
}

Serv::~Serv()
{
}

location::location(): path(""), directory_listing(0), upload_path("")
{

}

location::~location(){

}
