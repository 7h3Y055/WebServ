#include "webserv.hpp"



vector <Serv> servers;



int main(int ac, char **av)
{
    try{
        parse_config(ac, av);
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }


    int numbers_of_servers = 1;
    int ports[3] = {8081, 8081, 8083};
    std::vector<int> fds = _Create_servers(numbers_of_servers, ports);

    try
    {
        Request req;
        _Run_server(req, fds);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

}

