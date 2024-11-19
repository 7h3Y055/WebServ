```
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
```


int fd = socket(AF_INET, SOCK_STREAM, 0);


## socket(domain , type , protocol); ##

* socket is a file descriptor , can perform the read/write over this file desc.. ,
it create an end point to recieve and send data over the network;

* the other end points also open a file descriptor and connects to the first socket
to read/write , (this is a client);

* AF_INET stands for " Address Family - Internet ". It is used in socket programming to specify that the communication domain is the IPv4 protocol. When you create a socket with AF_INET, it indicates that the socket will be used for communication over an IPv4 network.

*
{
    SOCK_STREAN (stream socket) : uses TCP/IP, ip is responsible for data routing not data inetegrity
    ensure that the data sent and no data should be loses (TCP/IP concept).

    SOCK_DGRAM (datagram socket) : uses UDP , packet may not reach , but if reached, illbe error free (uses in streamin lives and videos) .
} 

Resume:
    * AF_INET is used for IPv4 addresses (e.g., 192.168.1.1).

    * When combined with SOCK_STREAM, it typically indicates the use of the TCP protocol for reliable, connection-oriented communication.


~ If you were to use AF_INET6, that would specify IPv6 addresses for communication.

the 0 represents the protocol to be used with the socket. When you pass 0, it tells the system to automatically choose the default protocol for the specified socket type.

* For SOCK_STREAM (which represents a TCP socket), 0 will default to the TCP protocol.

* For SOCK_DGRAM (which represents a UDP socket), 0 will default to the UDP protocol.

Specifying 0 is common practice when you don't need to specify an explicit protocol because SOCK_STREAM and SOCK_DGRAM inherently imply TCP and UDP, respectively.


## setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ##
