#include "webserv.hpp"

int main(int ac, char **av)
{
    try{
        
        Request req;


        std::string request1 = "POST /bla?q=1 HTT";
        std::string request2 = "P/1.1\r\nHost:     localhost:8080";
        std::string request3 = "\r\nUser-Agent: curl/7";
        std::string request4 = ".68.0\r\nAccept: */*\r\n";
        std::string request5 = "Content-Length: 9223372036854775807\r\n\r\n";
        // std::string request5 = "Transfer-Encoding:    chunked   \r\n\r\n";
        std::string request6 = "data1\r\ndata2\r\n";
        std::string request7 = "data3\r\n";
        std::string request8 = "data4\r\n";
        std::string request9 = "data5\r\ndata5\r\ndata5\r\ndata5\r\ndata5\r\n";
        std::string request10 = "END\r\n\r\n\r\n";
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

        // std::string request;
        // std::cout << "Go to http://localhost:80/" << std::endl;
        // while (std::getline(std::cin, request))
        // {
        //     request += "\r\n";


        //     req.fill_request(request);
        // }
        std::cout << "Method: " << req.get_method() << std::endl;
        std::cout << "URI: " << req.get_URI() << std::endl;
        std::cout << "Version: " << req.get_version() << std::endl;
        std::cout << "Host: " << req.get_Host() << std::endl;
        std::cout << "File name: " << req.get_file_name() << std::endl;
        std::cout << "Transfer mechanism: '" << req.get_transfer_mechanism() << "'" << std::endl;
        if (req.get_transfer_mechanism() == "Fixed")
            std::cout << "Fixed length: '" << req.get_fixed_length() << "'" << std::endl;
        std::cout << "Headers: {" << std::endl;
        std::map<std::string, std::string> headers = req.get_headers();
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++){
            std::cout << "'" << it->first << "': '" << it->second << "'\n";
        }
        std::cout << "}\nBody: {\n" << req.get_body() << "}" << std::endl;
        // std::cout <<  req.get_body() << std::endl;
    }
    catch (std::exception &e){
        std::cout << e.what() << std::endl;
    }

}