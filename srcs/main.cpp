#include "webserv.hpp"

int main(int ac, char **av)
{
    try{
        
        Request req;


        std::string request1 = "POST /bla?q=1 HTT";
        std::string request2 = "P/1.1\r\nHost:     localhost:8080\r\n";
        std::string request4 = "\r\n\r\n";
        std::string request3 = "Transfer-Encoding:    chunked";
        std::string request5 = "1\r\n";
        std::string request6 = "data\r\n";
        std::string request7 = "2\r\n";
        std::string request8 = "abcd";
        std::string request9 = "efgh\r\n0\r\n";
        // std::string request3 = "Content-Length: 17";
        // std::string request5 = "A\r\n";
        // std::string request6 = "B\r\n";
        // std::string request7 = "C\r\n";
        // std::string request8 = "DATA\r\n";
        // std::string request9 = "END";
        req.fill_request(request1);
        req.fill_request(request2);
        req.fill_request(request3);
        req.fill_request(request4);
        req.fill_request(request5);
        req.fill_request(request6);
        req.fill_request(request7);
        req.fill_request(request8);
        req.fill_request(request9);
        req.request_complete();

        // std::string request;
        // std::cout << "Go to http://localhost:80/" << std::endl;
        // while (std::getline(std::cin, request))
        // {
        //     request += "\r\n";


        //     req.fill_request(request);
        // }
        // std::cout << "Method: " << req.get_method() << std::endl;
        // std::cout << "URI: " << req.get_URI() << std::endl;
        // std::cout << "Version: " << req.get_version() << std::endl;
        // std::cout << "Host: " << req.get_Host() << std::endl;
        // std::cout << "File name: " << req.get_file_name() << std::endl;
        std::cout << "Transfer mechanism: '" << req.get_transfer_mechanism() << "'" << std::endl;
        if (req.get_transfer_mechanism() == "Fixed")
            std::cout << "Fixed length: '" << req.get_fixed_length() << "'" << std::endl;
        // std::cout << "Headers: {" << std::endl;
        // std::map<std::string, std::string> headers = req.get_headers();
        // for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++){
        //     std::cout << "'" << it->first << "': '" << it->second << "'\n";
        // }
        // std::cout << "}" << std::endl;
        // std::cout << "Body: {\n" << req.get_body() << "}" << std::endl;
        std::cout <<  req.get_body() << std::endl;
    }
    catch (std::exception &e){
        std::cout << e.what() << std::endl;
    }

}