#include "webserv.hpp"

int main(int ac, char **av)
{
    std::vector <Serv> servers;
    try{
        servers = parse_config(ac, av);
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    
    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "SERVER " << (i + 1) << std::endl;
        std::cout << "port: " << servers[i].port << std::endl;
        std::cout << "host: " << servers[i].host << std::endl;
        std::cout << "server_name: " << servers[i].server_name << std::endl;
        std::cout << "root: " << servers[i].root << std::endl;
        std::cout << "client_max_body_size: " << servers[i].client_max_body_size << std::endl;
        std::cout << "error_pages: " << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = servers[i].error_pages.begin(); it != servers[i].error_pages.end(); ++it)
            std::cout << it->first << " => " << it->second << std::endl;
        std::cout << "locations: " << std::endl;
        for (size_t j = 0; j < servers[i].locations.size(); ++j){
            std::cout << "location " << j << std::endl;
            std::cout << "path: " << servers[i].locations[j].path << std::endl;
            std::cout << "methods: ";
            for (size_t k = 0; k < servers[i].locations[j].methods.size(); ++k)
                std::cout << servers[i].locations[j].methods[k] << " ";
            std::cout << std::endl;
            std::cout << "index: ";
            for (size_t k = 0; k < servers[i].locations[j].index.size(); ++k)
                std::cout << servers[i].locations[j].index[k] << " ";
            std::cout << std::endl;
            std::cout << "directory_listing: " << servers[i].locations[j].directory_listing << std::endl;
            std::cout << "upload_path: " << servers[i].locations[j].upload_path << std::endl;
            std::cout << "cgi: " << std::endl;
            for (size_t k = 0; k < servers[i].locations[j].cgi.size(); ++k){
                std::cout << "cgi " << k << std::endl;
                for (std::map<std::string, std::string>::const_iterator it = servers[i].locations[j].cgi[k].begin(); it != servers[i].locations[j].cgi[k].end(); ++it)
                    std::cout << it->first << " => " << it->second << std::endl;
            }
            std::cout << "redirection: " << servers[i].locations[j].redirection << std::endl;
        }
    }
    


    // std::string request;
    // Request req;
    // int numbers_of_servers = 1;
    // int ports[3] = {8081, 8082, 8083};
    // std::vector<int> fds = _Create_servers(numbers_of_servers, ports);

    // try
    // {
    //     _Run_server(req, fds);
    // }
    // catch (std::exception &e)
    // {
    //     std::cout << e.what() << std::endl;
    // }

}

