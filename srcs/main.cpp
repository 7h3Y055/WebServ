#include "webserv.hpp"

std::vector<int> _Create_servers(int nos, int *ports)
{
    std::vector<int> fds;
    int fd;
    struct sockaddr_in addr;
    int opt = 1;
    for (int i = 0; i < nos; i++)
    {
        try
        {
            fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd == -1)
                throw std::runtime_error("socket failed");
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
                throw std::runtime_error("setsockopt failed");
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(ports[i]);
            if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
                throw std::runtime_error("bind failed");
            if (listen(fd, 10) == -1)
                throw std::runtime_error("listen failed");
            fds.push_back(fd);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            if (fd != -1)
                close(fd);
        }
    }       
    return fds;
}

#define BUFFER_SIZE 4096

void _Print_req(Request &req)
{
    std::cout << "Method: " << req.get_method() << std::endl;
    std::cout << "URI: " << req.get_URI() << std::endl;
    std::cout << "Version: " << req.get_version() << std::endl;
    std::cout << "Host: " << req.get_Host() << std::endl;
    std::cout << "File name: " << req.get_file_name() << std::endl;
    std::cout << "Headers: {" << std::endl;
    std::map<std::string, std::string> headers = req.get_headers();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        std::cout << "'" << it->first << "': '" << it->second << "'\n";
    }
    std::cout << "}" << std::endl;
    std::cout << "Content Type: " << req.get_transfer_mechanism() << std::endl;
    std::cout << "Body: {" << req.get_body() << "}" << std::endl;
}

void _Run_server(Request &req, std::vector<int> fds)
{
    char buffer[BUFFER_SIZE];
    int new_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int ret;
    while (1)
    {
        new_fd = accept(fds[0], (struct sockaddr *)&addr, &addr_len);
        if (new_fd == -1)
            throw std::runtime_error("accept failed");
        ret = recv(new_fd, buffer, BUFFER_SIZE, 0);
        if (ret == -1)
            throw std::runtime_error("recv failed");
        if (ret == 0)
            break;
        buffer[ret] = '\0';
        std::string request(buffer);
        std::cout << "this is request == " << request << std::endl;
        std::cout << "-----------------------------------" << std::endl;
        req.fill_request(request);
        if (req.request_state() == HTTP_COMPLETE)
            _Print_req(req);
        std::cout << "-----------------------------------" << std::endl;
        close(new_fd);
    }
}

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

