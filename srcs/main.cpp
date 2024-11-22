#include "webserv.hpp"

vector <Serv> servers;

int main(int ac, char **av)
{
    try
    {
        parse_config(ac, av);
        _Create_Servers();
        _Run_Server();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

