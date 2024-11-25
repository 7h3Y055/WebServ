#pragma once



enum RequestState
{
    HTTP_REQUEST_LINE,
    HTTP_HEADER,
    HTTP_BODY,
    HTTP_COMPLETE,
    DONE
};

class Response;

class Serv;

class Request
{
    private:
        RequestState _request_state;
        int _Host_found;

        std::vector<char> _Buffer;
        std::string _Method;
        std::string _Transfer_Mechanism;
        long long _Fixed_length;
        std::string _File_name;
        std::string _URI;
        std::string _Version;
        std::string _Host;
        std::map<std::string, std::string> _Headers;
        std::string _Body_path;
        int server_index;
    public:
        int _fd;
        bool chunked_state;
        unsigned long long chunked_length;
        void fill_request(std::vector<char> &buf);
        Response *execute_request();

        Response *post_Response();

        RequestState &request_state();
        std::string &get_method();
        std::string &get_version();
        std::string get_file_name();
        long long &get_fixed_length();
        std::string &get_Host();
        std::string &get_URI();
        std::map<std::string, std::string> &get_headers();
        std::string &get_header(std::string key);
        std::string &get_body_path();
        std::string &get_transfer_mechanism();
        int get_server_index(){
            return server_index;
        }
        void set_server_index(int i){
            server_index = i;
        }


        Request();
        ~Request();
};

