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

#define BUFFER_SIZE 30000

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
    if (req.get_transfer_mechanism() == "Fixed")
        std::cout << "Content Length: " << req.get_fixed_length() << std::endl;
    std::cout << "Request State: " << (req.request_state() == 3? "DONE":"NOT DONE") << std::endl;
    std::cout << "Body: {";
     for (std::vector<char>::iterator it = req.get_body().begin(); it != req.get_body().end(); ++it) {
        std::ofstream outfile("a.png", std::ios_base::app);
        if (outfile.is_open()) {
            outfile << *it;
            outfile.close();
        } else {
            std::cerr << "Unable to open file";
        }
        if (std::isprint(static_cast<unsigned char>(*it))) {
            std::cout << *it;
        } else {
            switch (*it) {
                case '\n':
                    std::cout << "\\n";
                    break;
                case '\r':
                    std::cout << "\\r";
                    break;
                case '\t':
                    std::cout << "\\t";
                    break;
                default:
                    std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)*it;
                    break;
            }
        }
    }
    std::cout << "}" << std::endl;
}

void _Run_server(Request &req, std::vector<int> fds)
{
    char buffer[BUFFER_SIZE + 1];
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
        std::cout << "-----------------------------------" << std::endl;
        std::vector<char> buf(buffer, buffer + ret);
        req.fill_request(buf);
        _Print_req(req);
        std::cout << "-----------------------------------" << std::endl;
        close(new_fd);
    }
}
