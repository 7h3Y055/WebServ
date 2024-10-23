#pragma once

#include <iostream>
#include <string>
#include <unistd.h>      // execve, dup, dup2, pipe, fork, chdir, access, read, write, close
#include <cstring>      // strerror, gai_strerror
#include <cerrno>       // errno
#include <sys/types.h>  // socket, accept, listen, send, recv, bind, connect, setsockopt, getsockname, getprotobyname, fcntl, stat, opendir, readdir, closedir
#include <sys/socket.h> // socket, accept, listen, send, recv, bind, connect, setsockopt, getsockname, socketpair
#include <netinet/in.h> // htons, htonl, ntohs, ntohl
#include <netdb.h>      // getaddrinfo, freeaddrinfo, getprotobyname
#include <sys/select.h> // select
#include <poll.h>       // poll
#include <sys/epoll.h>  // epoll_create, epoll_ctl, epoll_wait
#include <signal.h>     // signal, kill
#include <sys/wait.h>   // waitpid
#include <fcntl.h>      // fcntl
#include <dirent.h>     // opendir, readdir, closedir
#include <sys/stat.h>   // stat
#include <fstream>      // ifstream
#include <sstream>      // istringstream
#include <cstdlib>      // atoi
#include <map>
#include <vector>
#include <deque>
#include <list>


