#include "webserv.hpp"

int main(int ac, char **av)
{
    try{
        
        Request req;


        std::string request1 = "\r\n\r\n\r\nGET / HTT";//P/1.1\r\nHost: localhost:8080\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
        std::string request2 = "P/1.1\r\nHost: localho";//st:8080\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
        std::string request3 = "st:8080\r\nUser-Agent: curl/7";//.68.0\r\nAccept: */*\r\n\r\n";
        std::string request4 = ".68.0\r\nAccept: */*\r\n\r\n";
        std::string request5 = "data1\r\n";
        std::string request6 = "data2\r\n";
        std::string request7 = "data3\r\n";
        std::string request8 = "data4\r\n";
        std::string request9 = "data5\r\ndata5\r\ndata5\r\ndata5\r\ndata5\r\n";
        std::string request10 = "END\r\n";
        req.fill_request(request1);
        req.fill_request(request2);
        req.fill_request(request3);
        req.fill_request(request4);
        req.fill_request(request5);
        req.fill_request(request6);
        req.fill_request(request7);
        req.fill_request(request8);
        req.fill_request(request9);
        req.fill_request(request10);

        // while (std::getline(std::cin, request))
        // {
        //     request += "\r\n";


            // req.fill_request(request);
        // }
        // if (!req.is_headers_complete())
            // return 0;
        std::cout << "Method: " << req.get_method() << std::endl;
        std::cout << "URI: " << req.get_URI() << std::endl;
        std::cout << "Version: " << req.get_version() << std::endl;
        std::cout << "File name: " << req.get_file_name() << std::endl;
        std::cout << "Headers: {" << std::endl;
        std::map<std::string, std::string> headers = req.get_headers();
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++){
            std::cout << "'" << it->first << "': '" << it->second << "'\n";
        }
        std::cout << "}\nBody: {\n" << req.get_body() << "}" << std::endl;
    }
    catch (std::exception &e){
        std::cout << e.what() << std::endl;
    }

}