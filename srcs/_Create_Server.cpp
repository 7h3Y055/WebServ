#include "webserv.hpp"


std::string _to_string(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

void _Create_Servers()
{
    std::map<int, std::vector<Serv> > sockets_servs;
    std::map<std::pair<std::string, int>, int> host_port_to_fd;
    int opt = 1;
    for (size_t i = 0; i < servers.size(); i++)
    {
        int port = servers[i].getPort();
        std::string host = servers[i].getHost();
        std::pair<std::string, int> host_port = std::make_pair(host, port);
        if (host_port_to_fd.find(host_port) == host_port_to_fd.end())
        {
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd == -1){
                // cerr << "\033[1;36m[WebServ]\033[0m " << "socket failed" << endl;
                servers[i].setFd(-1);
                continue;
            }
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            {
                close(fd);
                // cerr << "\033[1;36m[WebServ]\033[0m " << "setsockopt failed" << endl;
                servers[i].setFd(-1);
                continue;
            }
            struct addrinfo hints, *res;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;
            std::string port_str = _to_string(port);
            int get_addr = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
            if (get_addr != 0)
            {
                close(fd);
                std::cerr << "\033[1;36m[WebServ]\033[0m " << "\033[1;31mServer " << i + 1 << " [" << host << ":" << port << "] can't start cause: " << strerror(errno) << "\033[0m" << std::endl;
                servers[i].setFd(-1);
                continue;
            }
            if (bind(fd, res->ai_addr, res->ai_addrlen) == -1)
            {
                std::cerr << "\033[1;36m[WebServ]\033[0m " << "\033[1;31mServer " << i + 1 << " [" << host << ":" << port << "] can't start cause: " << strerror(errno) << "\033[0m" << std::endl;
                close(fd);
                freeaddrinfo(res);
                servers[i].setFd(-1);
                continue;
            }
            freeaddrinfo(res);
            if (listen(fd, SOMAXCONN) == -1)
            {
                close(fd);
                // cerr << "\033[1;36m[WebServ]\033[0m " << "listen failed" << endl;
                servers[i].setFd(-1);
                continue;
            }
            host_port_to_fd[host_port] = fd;
            servers[i].setFd(fd);
            sockets_servs[fd].push_back(servers[i]);
        }
        else
        {
            servers[i].setFd(host_port_to_fd[host_port]);
            sockets_servs[host_port_to_fd[host_port]].push_back(servers[i]);
        }
    }
}

std::vector<int> get_servers_same_fd(std::vector<Serv> &servers, int fd)
{
    std::vector<int> indexes;
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i].getFd() == fd)
        {
            indexes.push_back(i);
        }
    }
    return indexes;
}


int get_server_index_(string &host, int fd)
{
    std::vector<int> fds = get_servers_same_fd(servers, fd);
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i].getFd() == fd)
        {
            for (size_t k = 0; k < fds.size(); k++)
            {
                for (size_t j = 0; j < servers[fds[k]].getServerName().size(); j++)
                {
                    size_t pos = servers[fds[k]].getServerName()[j].find(":");
                    size_t pos2 = host.find(":");
                    if (pos2 < host.size()) // wen have a port
                    {
                        if (host.substr(pos2 + 1) == _to_string(servers[fds[k]].getPort()) &&
                            servers[fds[k]].getServerName()[j].substr(0, pos) == host.substr(0, pos2))   
                            {
                                return fds[k];
                            }
                    }
                    if (servers[fds[k]].getServerName()[j].substr(0, pos) == host.substr(0, pos2)) // wen have just a host
                    {
                        return fds[k];
                    }
                }
            }
            return i;
        }
    }
    return 0;
}

void    Client_desconnected(std::map<int, Client *> &clients, int &epoll_fd, int client_fd)
{
    if (clients[client_fd] == NULL)
        return;
    // cout << "\033[1;36m[WebServ]\033[0m " << "⬅ Client disconnected: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << endl;
    if (clients[client_fd]->get_req().get_body_path().size() != 0){
        remove(clients[client_fd]->get_req().get_body_path().c_str());
    }
    if (clients[client_fd]->cgi){ // kill child process
        kill(clients[client_fd]->cgi->get_cgi_child(), SIGKILL);
        waitpid(clients[client_fd]->cgi->get_cgi_child(), NULL, 0);
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
    delete clients[client_fd];
    clients.erase(client_fd);
    close(client_fd);
}

void _Check_for_timeout(std::map<int, Client *> &clients, int &epoll_fd)
{
    time_t current_time = time(NULL);
    std::map<int, Client *>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if ( it->second && current_time - it->second->get_last_read() > TIMEOUT)
        {
            // std::cout << "\033[1;36m[WebServ]\033[0m " << "Client timed out: " << it->second->get_ip() << ":" << it->second->get_port() << std::endl;
            if (it->second->get_req().get_body_path().size() != 0){
                remove(it->second->get_req().get_body_path().c_str());
            }
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
            delete it->second;
            close(it->first);
            clients.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void _Run_Server()
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        throw std::runtime_error("epoll_create failed");
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    std::vector<int> fds;
    int g_server = 0;

    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i].getFd() == -1)
            continue;
        std::cout << "\033[1;36m[WebServ]\033[0m " << "\033[1;32mServer " << i + 1 << " [" << servers[i].getHost() << ":" << servers[i].getPort() << "] started successfully\033[0m" << std::endl;
        g_server++;
        event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP;
        event.data.fd = servers[i].getFd();
        if(std::find(fds.begin(), fds.end(), servers[i].getFd()) == fds.end())
        {
            fds.push_back(servers[i].getFd());
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].getFd(), &event);
                // std::cerr << "\033[1;36m[WebServ]\033[0m " << "epoll_ctl failed: " << strerror(errno) << std::endl;
        }
    }
    if (!g_server)
        return ;

    std::map<int, Client *> clients;
    while (true)
    {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        if (num_events == -1){
            cerr << "\033[1;36m[WebServ]\033[0m " << "epoll_wait failed" << endl;
            continue;
        }
        _Check_for_timeout(clients, epoll_fd);
        for (int i = 0; i < num_events; i++)
        {
            if (std::find(fds.begin(), fds.end(), events[i].data.fd) != fds.end())
            {
                if (events[i].events & EPOLLIN)
                {
                    int client_fd = accept(events[i].data.fd, (struct sockaddr *)&addr, &addr_len);
                    if (client_fd == -1)
                    {
                        // std::cerr << "\033[1;36m[WebServ]\033[0m " << "accept failed: " << strerror(errno) << std::endl;
                        continue;
                    }
                    Client *client = new Client(client_fd, addr, -1);
                    client->header_flag = false;
                    client->cgi_header_flag = false;
                    event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLOUT;
                    event.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
                    {
                        delete client;
                        close(client_fd);
                        cerr << "\033[1;36m[WebServ]\033[0m " << "epoll_ctl failed" << endl;
                        continue;
                    }
                    clients[client_fd] = client;
                    clients[client_fd]->req._fd = events[i].data.fd;
                    // std::cout << "\033[1;36m[WebServ]\033[0m " << "➡ New connection from " << client->get_ip() << ":" << client->get_port() << std::endl;
                }
            }
            else
            {
                int client_fd = events[i].data.fd;
                try
                {
                    if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
                    {
                        Client_desconnected(clients, epoll_fd, client_fd);
                        continue;
                    }
                    if (events[i].events & EPOLLIN)
                    {
                        int ret = recv(client_fd, clients[client_fd]->get_buffer(), BUFFER_SIZE, 0);
                        if (ret <= 0)
                        {
                            Client_desconnected(clients, epoll_fd, client_fd);
                            continue;
                        }
                        clients[client_fd]->update_last_read();
                        clients[client_fd]->set_read_pos(clients[client_fd]->get_read_pos() + ret);
                        char *buffer = clients[client_fd]->get_buffer();
                        std::vector<char> buf(buffer, buffer + ret);

                        clients[client_fd]->req.fill_request(buf);

                    }
                    else if (events[i].events & EPOLLOUT && clients[client_fd] && clients[client_fd]->req.request_state() == HTTP_COMPLETE)
                    {
                        string file_path = get_CGI_script(clients[client_fd]->req.get_file_name(), clients[client_fd]->req.get_server_index(), 0);
                        location loc = get_location(file_path, servers[clients[client_fd]->req.get_server_index()]);

                        cout << "\033[1;36m[WebServ]\033[0m " << "Request: " << clients[client_fd]->req.get_method() << " " << clients[client_fd]->req.get_file_name() << " from " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << endl;
                        string pth = loc.getRoot() + file_path;
                        for (size_t i = 0; i < loc.getIndex().size(); i++)
                        {
                            std::string index_path = pth + "/" + loc.getIndex()[i];
                            if (access(index_path.c_str(), F_OK) == 0)
                            {
                                file_path = file_path + loc.getIndex()[i];
                                break;
                            }
                        }
                        clients[client_fd]->req.set_file_name(file_path);
                        


                        if (loc.getRedirection().size() > 0)
                        {
                            Response *res = create_redirection(loc, clients[client_fd]->req);
                            std::vector<char> response_binary = res->get_response();
                            ssize_t ret = send(client_fd, &(*response_binary.begin()), response_binary.size(), MSG_NOSIGNAL);
                            if (ret == -1)
                            {
                                // std::cerr << "\033[1;36m[WebServ]\033[0m " << "Send failed" << std::endl;
                            }
                            Client_desconnected(clients, epoll_fd, client_fd);
                            continue;
                        }

                        if(is_CGI(file_path, clients[client_fd]->req.get_server_index(), 0) 
                            && clients[client_fd]->req.get_method() != "DELETE")
                        {
                            char buffer[SEND_BUFFER_SIZE];
                            if (clients[client_fd]->cgi_header_flag == false)
                            {
                                if (clients[client_fd]->already_visite_cgi == false){
                                    clients[client_fd]->cgi = new CGI(clients[client_fd]->req, loc);
                                }
                                if (clients[client_fd]->cgi->execute(clients.at(client_fd)) == STILL_RUNNING)
                                    continue;
                                clients[client_fd]->path_file = clients[client_fd]->cgi->get_response();
                                clients[client_fd]->cgi_file_stream.open(clients[client_fd]->path_file.c_str(), std::ios::binary);
                                if (!clients[client_fd]->cgi_file_stream.is_open())
                                    throw 500;
                                clients[client_fd]->cgi_file_offset = 0;
                                clients[client_fd]->cgi_header_flag = true;
                                clients[client_fd]->sending_file = true;
                            }
                            else
                            {
                                std::memset(buffer, 0, SEND_BUFFER_SIZE);
                                clients[client_fd]->cgi_file_stream.seekg(clients[client_fd]->cgi_file_offset);
                                clients[client_fd]->cgi_file_stream.read(buffer, SEND_BUFFER_SIZE);
                                size_t bytes_read = clients[client_fd]->cgi_file_stream.gcount();
                                if (bytes_read == 0)
                                {
                                    Client_desconnected(clients, epoll_fd, client_fd);
                                }
                                else
                                {
                                    ssize_t ret = send(client_fd, buffer, bytes_read, MSG_NOSIGNAL);
                                    if (ret != -1)
                                    {
                                        clients[client_fd]->update_last_read();
                                    }
                                    if (ret == -1)
                                    {
                                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                        delete clients[client_fd];
                                        clients.erase(client_fd);
                                        close(client_fd);
                                        continue;
                                    }
                                    clients[client_fd]->cgi_file_offset += bytes_read;
                                }
                            }

                        }
                        else if (clients[client_fd]->req.get_method() == "GET" ) // GET
                        {
                            std::string resources = clients[client_fd]->req.get_file_name();
                            std::string root = loc.getRoot();
                            std::string path = root + resources;
                            if (access(path.c_str(), F_OK) == -1)
                            {
                                throw 404;
                            }
                            std::vector<std::string> index = loc.getIndex();
                            for (size_t i = 0; i < index.size(); i++)
                            {
                                std::string index_path = path + "/" + index[i];
                                if (access(index_path.c_str(), F_OK) == 0)
                                {
                                    path = index_path;
                                    break;
                                }
                            }
                            if (get_resources_type(path) == "directory")
                            {
                                if (path[path.size() - 1] != '/')
                                {
                                    Response *res = createResponse(301, &clients[client_fd]->req);
                                    res->set_header("Location", resources + '/');
                                    vector<char> response_binary = res->get_response();
                                    ssize_t ret = send(client_fd, &(*response_binary.begin()), response_binary.size(), MSG_NOSIGNAL);
                                    if (ret == -1)
                                    {
                                        // std::cerr << "\033[1;36m[WebServ]\033[0m " << "Send failed" << std::endl;
                                    }
                                    delete res;
                                    Client_desconnected(clients, epoll_fd, client_fd);
                                    continue;
                                }
                                if (!loc.getDirectoryListing())
                                {
                                    throw 403;
                                }
                                std::vector<std::string> directory_content = get_directory_content(path);
                                if (access(path.c_str(), R_OK) == -1)
                                {
                                    throw 403;
                                }
                                Response *res = new Response(clients[client_fd]->req);
                                res->set_status_code(200);
                                res->set_status_message("OK");
                                std::string content = "<html><body><h1>Directory listing</h1><ul>";
                                for (size_t i = 0; i < directory_content.size(); i++)
                                {
                                    content += "<li><a href=\"" + directory_content[i] + "\">" + directory_content[i];
                                    if (get_resources_type(path + "/" + directory_content[i]) == "directory")
                                    {
                                        content += "/";
                                    }
                                    content += "</a></li>";
                                }
                                content += "</ul></body></html>";
                                std::vector<char> body(content.begin(), content.end());
                                res->set_body(body);
                                std::vector<char> response_binary = res->get_response();
                                ssize_t ret = send(client_fd, &(*response_binary.begin()), response_binary.size(), MSG_NOSIGNAL);
                                if (ret == -1)
                                {
                                    // std::cerr << "\033[1;36m[WebServ]\033[0m " << "Send failed" << std::endl;
                                }
                                delete res;
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                delete clients[client_fd];
                                clients.erase(client_fd);
                                close(client_fd);
                            }

                            else
                            {
                                char buffer[SEND_BUFFER_SIZE];
                                if (clients[client_fd]->header_flag == false)
                                {
                                    clients[client_fd]->file_stream.open(path.c_str(), std::ios::binary);
                                    if (!clients[client_fd]->file_stream.is_open())
                                        throw 403;
                                    clients[client_fd]->file_offset = 0;
                                    clients[client_fd]->sending_file = true;
                                    clients[client_fd]->header_flag = true;
                                    std::vector<char> header = generate_header(clients[client_fd]->file_stream, path);
                                    for (size_t i = 0; i < header.size(); i++)
                                        buffer[i] = header[i];
                                    ssize_t ret = send(client_fd, buffer, header.size(), MSG_NOSIGNAL);
                                    if (ret != -1)
                                    {
                                        clients[client_fd]->update_last_read();
                                    }
                                    if (ret == -1)
                                    {
                                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                        delete clients[client_fd];
                                        clients.erase(client_fd);
                                        close(client_fd);
                                        continue;
                                    }
                                }
                                else
                                {
                                    std::memset(buffer, 0, SEND_BUFFER_SIZE);
                                    clients[client_fd]->file_stream.seekg(clients[client_fd]->file_offset);
                                    clients[client_fd]->file_stream.read(buffer, SEND_BUFFER_SIZE);
                                    size_t bytes_read = clients[client_fd]->file_stream.gcount();
                                    if (bytes_read == 0)
                                    {
                                        Client_desconnected(clients, epoll_fd, client_fd);

                                    }
                                    else
                                    {
                                        ssize_t ret = send(client_fd, buffer, bytes_read, MSG_NOSIGNAL); // HNA MSG_NOSIGNAL
                                        if (ret != -1)
                                        {
                                            clients[client_fd]->update_last_read();
                                        }
                                        if (ret == -1)
                                        {
                                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                            delete clients[client_fd];
                                            clients.erase(client_fd);
                                            close(client_fd);
                                            continue;
                                        }
                                        clients[client_fd]->file_offset += bytes_read;
                                    }
                                }
                            }
                        }
                        else // for POST & DELETE 
                        {
                            Response *res = clients[client_fd]->req.execute_request();
                            std::vector<char> response_binary = res->get_response();
                            ssize_t ret = send(client_fd, &(*response_binary.begin()), response_binary.size(), MSG_NOSIGNAL);
                            if (ret == -1)
                            {
                                // std::cerr << "\033[1;36m[WebServ]\033[0m " << "Send failed" << std::endl;
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                delete clients[client_fd];
                                clients.erase(client_fd);
                                close(client_fd);
                                continue;
                            }
                            if (clients[client_fd]->get_req().get_body_path().size() != 0){
                                remove(clients[client_fd]->get_req().get_body_path().c_str());
                            }
                            Client_desconnected(clients, epoll_fd, client_fd);

                        }
                    }
                }
                catch(const int &code)
                {
                    // std::cerr << "\033[1;36m[WebServ]\033[0m " << "  HTTP code: " << code << " " << get_error_message(code) << '\n';
                    Response *res = createResponse(code, &clients[client_fd]->req);
                    ssize_t ret = send(client_fd, &(*res->get_response().begin()), res->get_response().size(), 0);
                    if (ret == -1)
                    {
                        // std::cerr << "\033[1;36m[WebServ]\033[0m " << "Send failed" << std::endl;
                    }
                    delete res;
                    Client_desconnected(clients, epoll_fd, client_fd);

                }
            }
        }
    }
}