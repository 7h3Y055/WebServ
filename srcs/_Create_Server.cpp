#include "webserv.hpp"

// void _Print_req(Request &req)
// {
//     std::cout << "Method: " << req.get_method() << std::endl;
//     std::cout << "URI: " << req.get_URI() << std::endl;
//     std::cout << "Version: " << req.get_version() << std::endl;
//     std::cout << "Host: " << req.get_Host() << std::endl;
//     std::cout << "File name: " << req.get_file_name() << std::endl;
//     std::cout << "Headers: {" << std::endl;
//     std::map<std::string, std::string> headers = req.get_headers();
//     for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
//     {
//         std::cout << "'" << it->first << "': '" << it->second << "'\n";
//     }
//     std::cout << "}" << std::endl;
//     std::cout << "Content Type: " << req.get_transfer_mechanism() << std::endl;
//     if (req.get_transfer_mechanism() == "Fixed")
//         std::cout << "Content Length: " << req.get_fixed_length() << std::endl;
//     std::cout << "Request State: " << (req.request_state() == 3? "DONE":"NOT DONE") << std::endl;
//     std::cout << "Body: {";
//      for (std::vector<char>::iterator it = req.get_body().begin(); it != req.get_body().end(); ++it) {
//         if (std::isprint(static_cast<unsigned char>(*it))) {
//             std::cout << *it;
//         } else {
//             switch (*it)
//             {
//                 case '\n':
//                     std::cout << "\\n";
//                     break;
//                 case '\r':
//                     std::cout << "\\r";
//                     break;
//                 case '\t':
//                     std::cout << "\\t";
//                     break;
//                 default:
//                     std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)*it;
//                     break;
//             }
//         }
//     }
//     std::cout << "}" << std::endl;
// }


void _Create_Servers()
{
    int fd = -1;
    int opt = 1;
    for (int i = 0; i < servers.size(); i++)
    {
        try 
        {
            int port = servers[i].getPort();
            std::string host = servers[i].getHost();
            std::string server_name = servers[i].getServerName()[0];
            // std::cout << "server : " << i << " port : |" << port << " host : |" << host << "|" ;
            // std::cout << " server_name : |" << server_name << "|" << std::endl;
            fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd == -1)
                throw std::runtime_error("Error: socket() failed");
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
                throw std::runtime_error("setsockopt failed");
            servers[i].addr.sin_family = AF_INET;
            servers[i].addr.sin_port = htons(port);
            servers[i].addr.sin_addr.s_addr = htonl(INADDR_ANY);
            // servers[i].addr.sin_addr.s_addr = inet_addr(host.c_str());
            if (bind(fd, (struct sockaddr *)&servers[i].addr, sizeof(servers[i].addr)) == -1)
                throw std::runtime_error("Error: bind() failed");
            if (listen(fd, 10) == -1)
                throw std::runtime_error("Error: listen() failed");
            int flags = fcntl(fd, F_GETFL, 0);
            if (flags == -1)
                throw std::runtime_error("fcntl F_GETFL failed");
            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
                throw std::runtime_error("fcntl F_SETFL failed");
            servers[i].setFd(fd);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            if (fd != -1)
                close(fd);
        }
    }
}

int get_server_index(int fd)
{
    for (int i = 0; i < servers.size(); i++)
    {
        if (servers[i].getFd() == fd)
            return i;
    }
    return -1;
}


struct Data
{
    int fd;
    Request req;
    std::ifstream file_stream;
    size_t file_offset;
    bool sending_file;
    // Other members...
};


void _Run_Server()
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        throw std::runtime_error("epoll_create failed");
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    for (int i = 0; i < servers.size(); i++)
    {
        event.events = EPOLLIN;
        event.data.fd = servers[i].getFd();
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].getFd(), &event) == -1)
            throw std::runtime_error("epoll_ctl failed");
    }

    std::map<int, Client *> clients;
    std::vector<int> fds;

    for (int i = 0; i < servers.size(); i++) {
        fds.push_back(servers[i].getFd()); }

    std::vector<int> clients_response;

    while (true)
    {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1)
            throw std::runtime_error("epoll_wait failed");
        for (int i = 0; i < num_events; i++)
        {
            if (std::find(fds.begin(), fds.end(), events[i].data.fd) != fds.end())
            {
                if (events[i].events & EPOLLIN)
                {
                    int client_fd = accept(events[i].data.fd, (struct sockaddr *)&addr, &addr_len);
                    if (client_fd == -1)
                    {
                        std::cerr << "accept failed: " << strerror(errno) << std::endl;
                        continue;
                    }
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags == -1)
                        throw std::runtime_error("fcntl F_GETFL failed");
                    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
                        throw std::runtime_error("fcntl F_SETFL failed");
                    int server_fd = events[i].data.fd;
                    int index = get_server_index(server_fd);
                    Client *client = new Client(client_fd, addr, index);
                    event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLOUT;
                    event.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
                    {
                        delete client;
                        close(client_fd);
                        throw std::runtime_error("epoll_ctl failed");
                    }
                    clients[client_fd] = client;
                    std::cout << "New connection from " << client->get_ip() << ":" << client->get_port() << std::endl;
                }
            }
            else
            {
                int client_fd = events[i].data.fd;
                // std::cout << "Client data available: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << std::endl;
                if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
                {
                    std::cout << "Client disconnected: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    delete clients[client_fd];
                    clients.erase(client_fd);
                    close(client_fd);
                    continue;
                }
                if (events[i].events & EPOLLIN )
                {
                    int ret = recv(client_fd, clients[client_fd]->get_buffer(), BUFFER_SIZE, 0);
                    if (ret == -1)
                        throw std::runtime_error("recv failed");
                    if (ret == 0)
                    {
                        std::cout << "Client disconnected: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << std::endl;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        delete clients[client_fd];
                        clients.erase(client_fd);
                        close(client_fd);
                        continue;
                    }
                    clients[client_fd]->set_read_pos(clients[client_fd]->get_read_pos() + ret);
                    char *buffer = clients[client_fd]->get_buffer();
                    std::vector<char> buf(buffer, buffer + ret);
                    clients[client_fd]->req.fill_request(buf);
                }
                else if (events[i].events & EPOLLOUT && clients[client_fd]->req.request_state() == HTTP_COMPLETE)
                {
                    try
                    {
                        Response *res = clients[client_fd]->req.execute_request();
                        std::vector<char> response_binary = res->get_response();
                        size_t start = 0;
                        size_t end = 0;
                        while (start < response_binary.size())
                        {
                            end = start + 1024;
                            if (end > response_binary.size())
                                end = response_binary.size();
                            send(client_fd, &(*response_binary.begin()) + start, end - start, 0);
                            start = end;
                        }
                        // send(client_fd, &(*response_binary.begin()), response_binary.size(), 0);
                        // send(client_fd, "\r\n", 2, 0);

                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        delete clients[client_fd];
                        clients.erase(client_fd);
                        close(client_fd);
                    }
                    catch(const int &code)
                    {
                        std::cerr << "HTTP code: " << code << " " << get_error_message(code) << '\n';
                        Response *res = createResponse(code, &clients[client_fd]->req);
                        send(client_fd, &(*res->get_response().begin()), res->get_response().size(), 0);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        delete clients[client_fd];
                        clients.erase(client_fd);
                        close(client_fd);
                    }
                }
            }
        }
    }
}