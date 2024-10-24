#include "webserv.hpp"

int main(int ac, char **av)
{
    std::string req1 = "GET / HTT";
    std::string req2 = "P/1.1\r\nHost: www.w3.o";
    std::string req3 = "rg\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html\r\nAccept-Langua";
    std::string req4 = "ge: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n";

    Request req;

    req.fill_request(req1);
    req.fill_request(req2);
    req.fill_request(req3);
    req.fill_request(req4);
    
    


}