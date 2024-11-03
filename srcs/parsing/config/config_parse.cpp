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
    if (strs[0] == "methods"){
        if (loc.already_set["methods"] == "true")
            throw std::runtime_error("Error: duplicate methods");
        loc.already_set["methods"] = "true";
        if (strs.size() == 2){
            parse_methods(loc.methods, strs[1]);
        }
    }
    else if (strs[0] == "index"){
        if (loc.already_set["index"] == "true")
            throw std::runtime_error("Error: duplicate index");
        loc.already_set["index"] = "true";
        if (strs.size() < 2)
            throw std::runtime_error("Error: invalid index");
        loc.index = split_string_with_multiple_delemetres(strs[1], "\t\n\v\f\r ,");
    }
    else if (strs[0] == "directory_listing"){
        if (loc.already_set["directory_listing"] == "true")
            throw std::runtime_error("Error: duplicate directory_listing");
        loc.already_set["directory_listing"] = "true";
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid directory_listing");
        if (strs[1] == "on")
            loc.directory_listing = 1;
        else if (strs[1] == "off")
            loc.directory_listing = 0;
        else
            throw std::runtime_error("Error: invalid directory_listing");
    }
    else if (strs[0] == "upload_path"){
        if (loc.already_set["upload_path"] == "true")
            throw std::runtime_error("Error: duplicate upload_path");
        loc.already_set["upload_path"] = "true";
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid upload_path");
        loc.upload_path = strs[1];
    }
    else if (strs[0] == "cgi"){
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid cgi");
        std::vector<string> cgi = split_string_with_multiple_delemetres(strs[1], "\t\n\v\f\r :");
        if (cgi.size() != 2)
            throw std::runtime_error("Error: invalid cgi");
        std::map<std::string, std::string> cgi_map;
        cgi_map[cgi[0]] = cgi[1];
        loc.cgi.push_back(cgi_map);
    }
    else if (strs[0] == "redirection"){
        if (strs.size() != 2)
            throw std::runtime_error("Error: invalid redirection");
        if (loc.already_set["redirection"] == "true")
            throw std::runtime_error("Error: duplicate redirection");
        loc.already_set["redirection"] = "true";
        loc.redirection = strs[1];
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
        throw std::runtime_error("Error: invalid server");
    trim(server_strs[0]);
    trim(server_strs[1]);

    if (server_strs[0] == "port"){
        if (server.already_set["port"] == "true")
            throw std::runtime_error("Error: duplicate port");
        server.already_set["port"] = "true";
        if (server_strs.size() < 2)
            throw std::runtime_error("Error: invalid port");
        for (size_t i = 0; i < server_strs[1].size(); i++)
            if (!isdigit(server_strs[1][i]))
                throw std::runtime_error("Error: invalid port");
        double n = atof(server_strs[1].c_str());
        if (n < 0 || n > 65535 || n != (int)n)
            throw std::runtime_error("Error: invalid port");
        server.port = n;
    }
    else if (server_strs[0] == "host"){
        if (server.already_set["host"] == "true")
            throw std::runtime_error("Error: duplicate host");
        server.already_set["host"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid host");
        server.host = server_strs[1];
    }
    else if (server_strs[0] == "server_name"){
        if (server.already_set["server_name"] == "true")
            throw std::runtime_error("Error: duplicate server_name");
        server.already_set["server_name"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid server_name");
        server.server_name = server_strs[1];
    }
    else if (server_strs[0] == "root"){
        if (server.already_set["root"] == "true")
            throw std::runtime_error("Error: duplicate root");
        server.already_set["root"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid root");
        server.root = server_strs[1];
    }
    else if (server_strs[0] == "client_max_body_size"){
        if (server.already_set["client_max_body_size"] == "true")
            throw std::runtime_error("Error: duplicate client_max_body_size");
        server.already_set["client_max_body_size"] = "true";
        if (server_strs.size() != 2)
            throw std::runtime_error("Error: invalid client_max_body_size");
        server.client_max_body_size = convert_to_byte(server_strs[1]);
    }
    else if (server_strs[0] == "error_page"){
        std::vector<string> error_page = split_string_with_multiple_delemetres(server_strs[1], "\t\n\v\f\r :");
        if (error_page.size() != 2)
            throw std::runtime_error("Error: invalid error_page");
        if (server.error_pages.find(error_page[0]) != server.error_pages.end())
            throw std::runtime_error("Error: duplicate error_page");
        server.error_pages[error_page[0]] = error_page[1];
    }
    else{
        throw std::runtime_error("Error: invalid server");
    }
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
        if (strs[0] == "location")
        {
            if (strs.size() != 3 || strs[2] != "{")
                throw std::runtime_error("Error: invalid location");
            location loc;
            loc.path = strs[1];
            read_location(file, line_n, loc);
            server.locations.push_back(loc);
        }
        else{
            init_server(server, strs);
        }
        
        line_n++;
    }
}

void set_default_error_pages(Serv &server){
    if (server.error_pages.find("400") == server.error_pages.end())
        server.error_pages["400"] = "error_pages/400.html";
    if (server.error_pages.find("401") == server.error_pages.end())
        server.error_pages["401"] = "error_pages/401.html";
    if (server.error_pages.find("403") == server.error_pages.end())
        server.error_pages["403"] = "error_pages/403.html";
    if (server.error_pages.find("404") == server.error_pages.end())
        server.error_pages["404"] = "error_pages/404.html";
    if (server.error_pages.find("405") == server.error_pages.end())
        server.error_pages["405"] = "error_pages/405.html";
    if (server.error_pages.find("500") == server.error_pages.end())
        server.error_pages["500"] = "error_pages/500.html";
    if (server.error_pages.find("501") == server.error_pages.end())
        server.error_pages["501"] = "error_pages/501.html";
    if (server.error_pages.find("505") == server.error_pages.end())
        server.error_pages["505"] = "error_pages/505.html";
    if (server.error_pages.find("414") == server.error_pages.end())
        server.error_pages["414"] = "error_pages/414.html";
    if (server.error_pages.find("413") == server.error_pages.end())
        server.error_pages["413"] = "error_pages/413.html";
    if (server.error_pages.find("409") == server.error_pages.end())
        server.error_pages["409"] = "error_pages/413.html";
}

std::vector<Serv> parse_config(int ac, char **av)
{
    std::vector<Serv> servers;
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
            if (server.port == 0 || server.host == "" || server.root == "" || (server.server_name == "" && server_n > 1))
                throw std::runtime_error("Error: invalid server");
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
            if (servers[i].server_name == servers[j].server_name) {
                throw std::runtime_error("Error: duplicate server_name");
            }
        }
    }
    
    return servers;
}


Serv::Serv(): port(0), host(""), server_name(""), root(""), client_max_body_size(-1)
{
}

Serv::~Serv()
{
}
