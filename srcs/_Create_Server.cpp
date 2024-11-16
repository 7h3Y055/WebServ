#include "../includes/webserv.hpp"




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
        try
        {
            int port = servers[i].getPort();
            std::string host = servers[i].getHost();
            std::pair<std::string, int> host_port = std::make_pair(host, port);

            if (host_port_to_fd.find(host_port) == host_port_to_fd.end())
            {
                int fd = socket(AF_INET, SOCK_STREAM, 0);
                if (fd == -1)
                    throw std::runtime_error("socket failed");
                if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
                {
                    close(fd);
                    throw std::runtime_error("setsockopt failed");
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
                    throw std::runtime_error("getaddrinfo failed");
                }
                if (bind(fd, res->ai_addr, res->ai_addrlen) == -1)
                {
                    close(fd);
                    freeaddrinfo(res);
                    throw std::runtime_error("bind failed");
                }
                freeaddrinfo(res);
                if (listen(fd, SOMAXCONN) == -1)
                {
                    close(fd);
                    throw std::runtime_error("listen failed");
                }
                if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
                {
                    close(fd);
                    throw std::runtime_error("fcntl failed");
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
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}


int get_server_index_(string &host)
{
    
    for (size_t i = 0; i < servers.size(); i++)
    {
        for (size_t j = 0; j < servers[i].getServerName().size(); j++)
        {
            size_t pos = servers[i].getServerName()[j].find(":");
            size_t pos2 = host.find(":");
            if (pos2 != string::npos)
            {
                if (host.substr(pos2 + 1) == _to_string(servers[i].getPort()) &&
                    servers[i].getServerName()[j].substr(0, pos) == host.substr(0, pos2)) // HERE warrning !!!!!!!!!!!!!!!
                    {
                        return i;
                    }
            }
            else if (servers[i].getServerName()[j].substr(0, pos) == host.substr(0, pos2))
            {
                return i;
            }
        }
    }
    return 0;
}

void _Check_for_timeout(std::map<int, Client *> &clients, int &epoll_fd)
{
    time_t current_time = time(NULL);
    std::map<int, Client *>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if ( it->second && current_time - it->second->get_last_read() > TIMEOUT)
        {
            std::cout << "Client timed out: " << it->second->get_ip() << ":" << it->second->get_port() << std::endl;
            if (it->second->get_req().get_body_path().size() != 0){
                cout << "Remove: " << it->second->get_req().get_body_path() << endl;
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

    for (size_t i = 0; i < servers.size(); i++)
    {
        event.events = EPOLLIN;
        event.data.fd = servers[i].getFd();
        if(std::find(fds.begin(), fds.end(), servers[i].getFd()) == fds.end())
        {
            fds.push_back(servers[i].getFd());
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].getFd(), &event) == -1)
                std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
        }
    }

    std::map<int, Client *> clients;

    std::vector<int> clients_response;
    while (true)
    {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        if (num_events == -1)
            throw std::runtime_error("epoll_wait failed");
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
                        std::cerr << "accept failed: " << strerror(errno) << std::endl;
                        continue;
                    }
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags == -1)
                        throw std::runtime_error("fcntl F_GETFL failed");
                    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
                        throw std::runtime_error("fcntl F_SETFL failed");
                    // int server_fd = events[i].data.fd;
                    // int index = get_server_index(server_fd);
                    Client *client = new Client(client_fd, addr, -1);
                    client->header_flag = false;
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
                    // std::cout << "new connection to server = " << servers[index].getPort() << servers[index].getServerName()[0] << std::endl;
                }
            }
            else
            {
                int client_fd = events[i].data.fd;
                // std::cout << "Client data available: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << std::endl;
                try
                {
                    if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
                    {
                        std::cout << "Client disconnected: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << std::endl;
                        if (clients[client_fd]->get_req().get_body_path().size() != 0){
                            cout << "Remove: " << clients[client_fd]->get_req().get_body_path() << endl;  
                            remove(clients[client_fd]->get_req().get_body_path().c_str());
                        }
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
                            std::cout << "Client disconnected: " << clients[client_fd]->get_ip() << ":" << clients[client_fd]->get_port() << std::endl;
                            if (clients[client_fd]->get_req().get_body_path().size() != 0){
                                cout << "Remove: " << clients[client_fd]->get_req().get_body_path() << endl;  
                                remove(clients[client_fd]->get_req().get_body_path().c_str());
                            }
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                            delete clients[client_fd];
                            clients.erase(client_fd);
                            close(client_fd);
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
                        // int server_fd = events[i].data.fd;


                        // int index = get_server_index_(server_fd, clients[client_fd]->req.get_Host());
                        // clients[client_fd]->req.set_server_index(index);
                        // std::cout << "server index == " << index << std::endl ;




                        if(is_CGI(clients[client_fd]->req.get_file_name(), clients[client_fd]->req.get_server_index(), 0) && clients[client_fd]->req.get_method() != "DELETE") //CGI
                        {

                            // cout << "CGI script : " << get_CGI_script(clients[client_fd]->req.get_file_name(), clients[client_fd]->req.get_server_index(), 0) << endl;

                            // cout << "CGI still not implemented !!!!!!!!!!!!!" << endl;

                            // cout << "CGI is done" << endl;
                            
                            location loc = get_location(get_CGI_script(clients[client_fd]->req.get_file_name(), clients[client_fd]->req.get_server_index(), 0), servers[clients[client_fd]->req.get_server_index()]);
                            CGI cgi(clients[client_fd]->req, loc);
                            cgi.execute();
                            Response* res = cgi.get_response();
                            std::vector<char> response_binary = res->get_response();


                            size_t start = 0;
                            size_t end = 0;
                            while (start < response_binary.size())
                            {
                                end = start + 2048;
                                if (end > response_binary.size())
                                    end = response_binary.size();
                                send(client_fd, &(*response_binary.begin()) + start, end - start, 0);
                                start = end;
                            }
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                            delete clients[client_fd];
                            clients.erase(client_fd);
                            close(client_fd);
                        }
                        else if (clients[client_fd]->req.get_method() == "GET") // GET
                        {
                            // bool is_cgi = false;
                            std::string resources = clients[client_fd]->req.get_URI();
                            location loc = find_best_location(servers[clients[client_fd]->req.get_server_index()].getLocations(), resources);
                            // std::cout << "location name == "<< loc.getPath() << std::endl;
                            std::string root = loc.getRoot();
                            // if (loc.getCgi().size() > 0)
                                // is_cgi = true;
                            std::string path = root + resources;
                            if (access(path.c_str(), F_OK) == -1)
                            {
                                std::cout << "have no access == " << path << std::endl;
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
                                    throw 301;
                                if (!loc.getDirectoryListing())
                                {
                                    std::cout << "no directory listing" << std::endl;
                                    throw 403;
                                }
                                std::vector<std::string> directory_content = get_directory_content(path);
                                if (access(path.c_str(), R_OK) == -1)
                                {
                                    std::cout << "permission denied of directory listing" << std::endl;
                                    throw 403;
                                }
                                Response *res = new Response(clients[client_fd]->req);
                                res->set_status_code(200);
                                res->set_status_message("OK");
                                std::string content = "<html><body><h1>Directory listing</h1><ul>";
                                for (size_t i = 0; i < directory_content.size(); i++)
                                {
                                    content += "<li><a href=\"" + directory_content[i] + "\">" + directory_content[i] + "</a></li>";
                                }
                                content += "</ul></body></html>";
                                std::vector<char> body(content.begin(), content.end());
                                res->set_body(body);
                                std::vector<char> response_binary = res->get_response();
                                size_t start = 0;
                                size_t end = 0;
                                while (start < response_binary.size())
                                {
                                    end = start + 2048;
                                    if (end > response_binary.size())
                                        end = response_binary.size();
                                    send(client_fd, &(*response_binary.begin()) + start, end - start, 0);
                                    start = end;
                                }
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
                                        throw 404;
                                    clients[client_fd]->file_offset = 0;
                                    clients[client_fd]->sending_file = true;
                                    clients[client_fd]->header_flag = true;
                                    std::vector<char> header = generate_header(clients[client_fd]->file_stream, path);
                                    for (size_t i = 0; i < header.size(); i++)
                                        buffer[i] = header[i];
                                    send(client_fd, buffer, header.size(), 0);
                                }
                                else
                                {
                                    std::memset(buffer, 0, SEND_BUFFER_SIZE); // WARRING !!!!!!!!!!
                                    clients[client_fd]->file_stream.seekg(clients[client_fd]->file_offset);
                                    clients[client_fd]->file_stream.read(buffer, SEND_BUFFER_SIZE);
                                    size_t bytes_read = clients[client_fd]->file_stream.gcount();
                                    // std::cout << "Bytes read: " << bytes_read << std::endl;
                                    // std::cout << "buffer: " << buffer << std::endl;
                                    if (bytes_read == 0)
                                    {
                                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                        delete clients[client_fd];
                                        clients.erase(client_fd);
                                        close(client_fd);
                                    }
                                    else
                                    {
                                        ssize_t ret = send(client_fd, buffer, bytes_read, 0);
                                        if (ret != -1)
                                        {
                                            clients[client_fd]->update_last_read();
                                        }
                                        clients[client_fd]->file_offset += bytes_read;
                                    }
                                }
                            }
                        }
                        else // for POST & DLETE 
                        {
                            Response *res = clients[client_fd]->req.execute_request();
                            std::vector<char> response_binary = res->get_response();
                            size_t start = 0;
                            size_t end = 0;
                            while (start < response_binary.size())
                            {
                                end = start + 2048;
                                if (end > response_binary.size())
                                    end = response_binary.size();
                                send(client_fd, &(*response_binary.begin()) + start, end - start, 0);
                                start = end;
                            }
                            if (clients[client_fd]->get_req().get_body_path().size() != 0){
                                cout << "Remove: " << clients[client_fd]->get_req().get_body_path() << endl;  
                                remove(clients[client_fd]->get_req().get_body_path().c_str());
                            }
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                            delete clients[client_fd];
                            clients.erase(client_fd);
                            close(client_fd);
                        }
                    }
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