#pragma one

enum RequestState
{
    HTTP_REQUEST_LINE,
    HTTP_HEADER,
    HTTP_BODY
};

class Request
{
    private:
        int _request_state;
        bool _is_request_CGI;
        int _Host_found;

        std::string _Buffer;
        std::string _Method;
        std::string _File_name;
        std::string _URI;
        std::string _Version;
        std::string _Host;
        std::map<std::string, std::string> _Headers;
        std::string _Body;
    public:


        void fill_request(std::string request);

        int request_state();
        bool is_request_CGI();
        std::string get_method();
        std::string get_version();
        std::string get_file_name();
        std::string get_Host();
        std::string get_URI();
        std::map<std::string, std::string> get_headers();
        std::string get_header(std::string key);
        std::string get_body();
        // bool check_request();


        Request();
        ~Request();
        Request(const Request& other);
        Request& operator=(const Request& other);
        Request(Request& other);
        Request& operator=(Request& other);
};

