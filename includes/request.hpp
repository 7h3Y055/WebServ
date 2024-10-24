#pragma one



class Request
{
    private:
        bool _is_request_complete;
        bool _is_request_CGI;

        std::string _Buffer;
        std::string _Method;
        std::string _File_name;
        std::string _URI;
        std::string _Host;
        std::map<std::string, std::string> _Headers;
        std::string _Body;
    public:


        void fill_request(std::string request);

        bool is_request_complete();
        bool is_request_CGI();
        std::string get_method();
        std::string get_file_name();
        std::string get_URI();
        std::string get_host();
        std::map<std::string, std::string> get_headers();
        std::string get_header(std::string key);
        std::string get_body();



        Request();
        ~Request();
        Request(const Request& other);
        Request& operator=(const Request& other);
        Request(Request& other);
        Request& operator=(Request& other);
};

