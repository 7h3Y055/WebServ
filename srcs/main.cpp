#include "webserv.hpp"

int main(int ac, char **av)
{
    try{
        
        std::string request;
        Request req;
        while (std::getline(std::cin, request))
        {
            request += "\r\n";


            req.fill_request(request);
            if (req.is_request_complete())
                break;
        }

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
        std::cerr << e.what() << std::endl;
    }

}