#pragma once


#include <vector>
#include <map>

class location
{
    private:
        /* data */
    public:
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
        unsigned int port;
        std::string host;
        std::string server_name;
        std::string root;
        std::string client_max_body_size;
        std::vector<location> locations;
        std::map <std::string, std::string> error_pages;


        Serv(/* args */);
        ~Serv();
};


std::vector<Serv> parse_config(int ac, char **av);

