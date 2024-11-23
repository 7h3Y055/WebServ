#pragma once

#include "webserv.hpp"


#define BUFFER_SIZE 2048
#define MAX_EVENTS 10


class CGI;
class Client
{
    private:
        int _fd;
        struct sockaddr_in _address;
        std::string _ip;
        int _port;

        char _buffer[BUFFER_SIZE];
        size_t _read_pos;
        size_t _bytes_received;
        size_t _bytes_sent;

        RequestState _state;
        int server_index;
        int server_fd;
        
        time_t _last_read;
    public:
        CGI *cgi;
        bool already_visite_cgi;
        bool cgi_done;
        time_t cig_start_time;
        Client(int fd, struct sockaddr_in address, int index)
        {
            cgi = NULL;
            already_visite_cgi = false;
            cgi_done = false;
            cig_start_time = 0;
            _fd = fd;
            _address = address;
            // _ip = inet_ntoa(address.sin_addr);
            _port = ntohs(address.sin_port);
            _read_pos = 0;
            _last_read = time(NULL);
            _bytes_received = 0;
            _bytes_sent = 0;
            _state = HTTP_REQUEST_LINE;
            server_index = index;
            req.set_server_index(index);
        }
        ~Client() {
            if (cgi)
                delete cgi;
            close(_fd);
        }

        void update_last_read() {
            _last_read = time(NULL);
        }
        time_t get_last_read() {
            return _last_read;
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
        void execute(){
            res = req.execute_request();
        }
        
        int get_server_index(){
            return server_index;
        }
        std::string &get_Host(){
            return req.get_Host();
        }
        // RequestState get_req_state(){
        //     return req.request_state();
        // }


        // no need for setters and getter for req and res !!

        Request req;
        Response *res;

        void set_server_index(int i){
            server_index = i;
            req.set_server_index(i);
        }

        void fill_request(std::vector<char> &buf){
            req.fill_request(buf);
        }

        void set_server_fd(int fd){
            server_fd = fd;
        }
        int get_server_fd(){
            return server_fd;
        }

        Request &get_req(){
            return req;
        }
        Response *get_Res(){
            return res;
        }

        std::ifstream file_stream;
        long long file_offset;
        bool sending_file;

        bool header_flag;
        std::string path_file;

        std::ifstream cgi_file_stream;
        bool cgi_header_flag;
        long long cgi_file_offset;
};