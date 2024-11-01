#include "webserv.hpp"

int main(int ac, char **av)
{
    std::string request;
    Request req;
    int numbers_of_servers = 1;
    int ports[3] = {8081, 8082, 8083};
    std::vector<int> fds = _Create_servers(numbers_of_servers, ports);

    try
    {
        _Run_server(req, fds);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

}

