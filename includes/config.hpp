#pragma once


#include <vector>
#include <map>

class location
{
    private:
        /* data */
    public:
        std::map<std::string, std::string> already_set;
        std::string path;
        std::vector<std::string> methods;
        std::vector<std::string> index;
        int directory_listing;
        std::string upload_path;
        std::vector<std::map<std::string, std::string> > cgi;
        std::string redirection;
};




class Serv
{
    private:
        
    public:
        std::map<std::string, std::string> already_set;
        unsigned int port;
        std::string host;
        std::string server_name;
        std::string root;
        unsigned long long client_max_body_size;
        std::vector<location> locations;
        std::map <std::string, std::string> error_pages;


        Serv(/* args */);
        ~Serv();
};


std::vector<Serv> parse_config(int ac, char **av);

