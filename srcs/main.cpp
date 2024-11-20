#include "webserv.hpp"

vector <Serv> servers;

int main(int ac, char **av)
{
    try
    {
        parse_config(ac, av);
        // cout << get_CGI_script("/cgibin/x.php/test/path/info", 0, 0) << endl;
        _Create_Servers();
        _Run_Server();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (const int &c)
    {
        std::cout << "HTTP code: " << c << std::endl;
    }

}

