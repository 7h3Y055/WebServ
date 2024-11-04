#pragma one



#define CLIENT_BODY_SIZE 100000000


enum RequestState
{
    HTTP_REQUEST_LINE,
    HTTP_HEADER,
    HTTP_BODY,
    HTTP_COMPLETE
};

class Response;

class Serv;

class Request
{
    private:
        int _request_state;
        bool _is_request_CGI;
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
        std::vector<char> _Body;
    public:

        void fill_request(std::vector<char> &buf);
        Response &execute_request();


        // Serv config;

        int request_state();
        bool is_request_CGI();
        std::string get_method();
        std::string get_version();
        std::string get_file_name();
        long long get_fixed_length();
        std::string get_Host();
        std::string get_URI();
        std::map<std::string, std::string> get_headers();
        std::string get_header(std::string key);
        std::vector<char> &get_body();
        std::string get_transfer_mechanism();


        Request();
        ~Request();
};

