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
            switch (*it)
            {
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

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

class Server
{
    private:
        int _fd;
    public:

};

class Client
{
    private:
        int _fd;
        struct sockaddr_in _address;
        std::string _ip;
        int _port;

        char _buffer[BUFFER_SIZE];
        size_t _read_pos;
        time_t _last_read;
        size_t _bytes_received;
        size_t _bytes_sent;

        RequestState _state;

    public:
        Client(int fd, struct sockaddr_in address)
        {
            _fd = fd;
            _address = address;
            _ip = inet_ntoa(address.sin_addr);
            _port = ntohs(address.sin_port);
            _read_pos = 0;
            _last_read = time(NULL);
            _bytes_received = 0;
            _bytes_sent = 0;
            _state = HTTP_REQUEST_LINE;
        }
        ~Client() {
            close(_fd);
        }
        int get_fd() {
            return _fd;
        }
        std::string get_ip() {
            return _ip;
        }
        int get_port() {
            return _port;
        }
        struct sockaddr_in get_address() {
            return _address;
        }
        char *get_buffer() {
            return _buffer;
        }
        size_t get_read_pos() {
            return _read_pos;
        }
        void set_read_pos(size_t pos){
            _read_pos = pos;
        }
        time_t get_last_read() {
            return _last_read;
        }
        size_t get_bytes_received() {
            return _bytes_received;
        }
        void set_bytes_received(size_t bytes) {
            _bytes_received = bytes;
        }
        size_t get_bytes_sent() {
            return _bytes_sent;
        }
        void set_bytes_sent(size_t bytes){
            _bytes_sent = bytes;
        }
        RequestState get_state() {
            return _state;
        }
        void set_state(RequestState state){
            _state = state;
        }

        Request req;
};

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
                         << ":" << clients[client_fd]->get_port() << std::endl;
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
                    clients[client_fd]->req.fill_request(buf);
                    if (clients[client_fd]->req.request_state() == HTTP_COMPLETE)
                    {
                        std::cout << "-----------------------------------" << std::endl;
                        // Response &res = clients[client_fd]->req.execute_request();
                        // std::vector<char> response_binary = res.get_response();
                        // response_binary send to client
                        _Print_req(clients[client_fd]->req);
                        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
                        send(client_fd, response.c_str(), response.size(), 0);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        delete clients[client_fd];
                        clients.erase(client_fd);
                        close(client_fd);
                        std::cout << "-----------------------------------" << std::endl;
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