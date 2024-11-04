#include "webserv.hpp"


int main(int ac, char **av)
{
    std::vector <Serv> servers;
    std::string request;
    Request req;

    try
    {
        servers = parse_config(ac, av);
        std::vector<Serv> _Servers = _Create_Servers(servers);
        _Run_Server(req, _Servers);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

