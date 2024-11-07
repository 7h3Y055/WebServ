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
    std::cout << "Body: { " << req.get_body_path() << "}" << std::endl;
}


int get_server_index(std::string Host){
    return 0;
}

void _Run_server(Request &req, std::vector<int> fds)
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        throw std::runtime_error("epoll_create failed");
    
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    for (int i = 0; i < fds.size(); i++)
    {
        event.events = EPOLLIN;
        event.data.fd = fds[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fds[i], &event) == -1)
            throw std::runtime_error("epoll_ctl failed");
    }
    
    std::map<int, Client *> clients;

    int k = 0;
    while (true)
    {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        
        if (num_events == -1)
            throw std::runtime_error("epoll_wait failed");

        for (int i = 0; i < num_events; i++)
        {
            // add new connnection 
            if (std::find(fds.begin(), fds.end(), events[i].data.fd) != fds.end())
            {
                if (events[i].events == EPOLLIN)
                {
                    int client_fd = accept(events[i].data.fd, (struct sockaddr *)&addr, &addr_len);
                    if (client_fd == -1)
                        throw std::runtime_error("accept failed");
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                    Client *client = new Client(client_fd, addr);
                    event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
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
            // data
            else
            {
                int client_fd = events[i].data.fd;
                std::cout << "Client data available: " << clients[client_fd]->get_ip() 
                         << ":/" << clients[client_fd]->get_port() << std::endl;
                // remove client
                if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
                {
                    std::cout << "Client disconnected: " << clients[client_fd]->get_ip() 
                             << ":" << clients[client_fd]->get_port() << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    delete clients[client_fd];
                    clients.erase(client_fd);
                    close(client_fd);
                    continue;
                }
                if (events[i].events & EPOLLIN)
                {
                    int ret = recv(client_fd, clients[client_fd]->get_buffer(), BUFFER_SIZE, 0);
                    if (ret == -1)
                        throw std::runtime_error("recv failed");
                    if (ret == 0)
                    {
                        std::cout << "Client disconnected: " << clients[client_fd]->get_ip() 
                             << ":" << clients[client_fd]->get_port() << std::endl;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        delete clients[client_fd];
                        clients.erase(client_fd);
                        close(client_fd);
                        continue;
                    }
                    clients[client_fd]->set_bytes_received(clients[client_fd]->get_bytes_received() + ret);
                    char *buffer = clients[client_fd]->get_buffer();
                    std::vector<char> buf(buffer, buffer + ret);

                    try
                    {
                        clients[client_fd]->fill_request(buf);
                        if (clients[client_fd]->get_req_state() == HTTP_COMPLETE)
                        {
                            std::cout << "-----------------------------------" << std::endl;
                            clients[client_fd]->set_sever_index(get_server_index(clients[client_fd]->get_Host()));
                            clients[client_fd]->execute();
                            // send it chunked if needed
                            send(client_fd, &(*clients[client_fd]->get_Res()->get_response().begin()), clients[client_fd]->get_Res()->get_response().size(), 0);
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                            delete clients[client_fd];
                            clients.erase(client_fd);
                            close(client_fd);
                        }
                    }
                    catch(const int &code)
                    {
                        std::cerr << "HTTP code: " << code << " " << get_error_message(code) << '\n';
                        Response *res = createResponse(code, &clients[client_fd]->get_Req());
                        send(client_fd, &(*res->get_response().begin()), res->get_response().size(), 0);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        delete clients[client_fd];
                        clients.erase(client_fd);
                        close(client_fd);
                    }
                    

                }
                // usleep(100000);
            }
        }
    }
}











/*
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

*/









// void _Run_server(Request &req, std::vector<int> fds)
// {
//     int epoll_fd = epoll_create1(0);
//     if (epoll_fd == -1)
//         throw std::runtime_error("epoll_create failed");
    
//     struct epoll_event event;
//     struct epoll_event events[MAX_EVENTS];
//     struct sockaddr_in addr;

//     for (int i = 0; i < fds.size(); i++)
//     {
//         event.events = EPOLLIN;
//         event.data.fd = fds[i];
//         if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fds[i], &event) == -1)
//             throw std::runtime_error("epoll_ctl failed");
//     }
    
//     std::map<int, Client *> clients;

//     while (true)
//     {
//         int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); 
        
//         if (num_events == -1)
//             throw std::runtime_error("epoll_wait failed");
//         for (int i = 0; i < num_events; i++)
//         {
//             if (events[i].events == EPOLLIN)
//             {
//                 if (std::find(fds.begin(), fds.end(), events[i].data.fd) != fds.end())
//                 {
//                     int fd = accept(events[i].data.fd, (struct sockaddr *)&addr, (socklen_t *)&addr);
//                     if (fd == -1)
//                         throw std::runtime_error("accept failed");
//                     // Client *client = new Client(fd, addr);
//                     // event.events = EPOLLIN;
//                     // event.data.fd = fd;
//                     // if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
//                     //     throw std::runtime_error("epoll_ctl failed");
//                     // clients[fd] = client;
//                     std::cout << "New connection from " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
//                     // std::cout << "New connection from " << client->get_ip() << ":" << client->get_port() << std::endl;
                

//                 }
//             }
//         }
//     }
// }