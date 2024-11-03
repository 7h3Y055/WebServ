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
        // for (size_t i = 0; i < strs.size(); ++i){
        //     if (strs[i] != "GET" && strs[i] != "POST" && strs[i] != "DELETE")
        //         throw std::runtime_error("Error: invalid method");
        //     methods.push_back(strs[i]);
        // }
    }
    // for (std::vector<string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
    //     std::cout << *it << std::endl;
    // }
    exit(0);
}

void init_location(location &loc, std::vector<string> &strs)
{
    if (strs.size() > 2)
        for (size_t i = 2; i < strs.size(); ++i)
            strs[1] += "=" + strs[i];



    if (strs[0] == "methods"){
        parse_methods(loc.methods, strs[1]);
    }
    // else if (strs[0] == "index"){

    // }
    // else if (strs[0] == "directory_listing"){

    // }
    // else if (strs[0] == "upload_path"){

    // }
    // else if (strs[0] == "cgi"){

    // }
    // else if (strs[0] == "redirection"){

    // }
    // else
    //     throw std::runtime_error("Error: invalid location");
}

void read_location(std::ifstream &file, size_t &line_n, location &loc)
{
    std::string line;
    std::vector<string> strs;
    while (std::getline(file, line))
    {
        trim(line);
        if (just_spaces(line))
            continue;
        if (line == "}")
            return;

        strs = split_string_with_multiple_delemetres(line, "=");
        init_location(loc, strs);
        // std::cout << strs[0] << "\t\t" << strs[1] << std::endl;
        
        line_n++;
    }
}

void read_server(std::ifstream &file, size_t &line_n, Serv &server)
{
    std::string line;
    std::vector<string> strs;
    while (std::getline(file, line))
    {
        trim(line);
        if (just_spaces(line))
            continue;
        if (line == "}SERVER")
            return;
        strs = split_string_with_multiple_delemetres(line, "\t\n\v\f\r ");
        if (strs[0] == "location")
        {
            if (strs.size() != 3 || strs[2] != "{")
                throw std::runtime_error("Error: invalid location");
            location loc;
            read_location(file, line_n, loc);
            server.locations.push_back(loc);
        }
        
        line_n++;
    }
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
    while (std::getline(file, line))
    {
        if (just_spaces(line))
            continue;
        if (line == "SERVER{")
        {
            Serv server;
            read_server(file, line_n, server);
            servers.push_back(server);
        }
        else if (!line.empty()){
            throw std::runtime_error("Error: invalid config file");
        }
        line_n++;
    }
    return servers;
}


Serv::Serv(/* args */)
{
}

Serv::~Serv()
{
}
