#pragma once


#include <vector>
#include <map>

class location
{
    private:
        std::string path;
        std::vector<std::string> methods;
        std::vector<std::string> index;
        bool directory_listing;
        std::string upload_path;
        std::map<std::string, std::string> cgi;
        std::map<int, std::string> redirection;
        std::string root;
    public:
        std::map<std::string, std::string> already_set;
        // Getters
        std::string &getPath() { return path; }
        std::vector<std::string> &getMethods() { return methods; }
        std::vector<std::string> &getIndex() { return index; }
        bool &getDirectoryListing() { return directory_listing; }
        std::string &getUploadPath() { return upload_path; }
        std::map<std::string, std::string> &getCgi() { return cgi; }
        std::map<int, std::string> &getRedirection() { return redirection; }
        std::string &getRoot() { return root; }

        // Setters
        void setPath(const std::string &p) { path = p; }
        void setIndex(const std::vector<std::string> &i) { index = i; }
        void setDirectoryListing(int dl) { directory_listing = dl; }
        void setUploadPath(const std::string &up) { upload_path = up; }
        void setRoot(const std::string &r) { root = r; }
        // void setRedirection(const int code, const std::string &r) { redirection[code] = r; }

        location();
        ~location();
};




class Serv
{
    private:
        int port;
        std::string host;
        std::vector<std::string> server_name;
        std::string root;
        double client_max_body_size;
        std::vector<location> locations;
        std::map <std::string, std::string> error_pages;
        
    public:
        std::map<std::string, std::string> already_set;
        // Getters
        int &getPort() { return port; }
        std::string &getHost() { return host; }
        std::vector<std::string> &getServerName() { return server_name; }
        void setRoot(std::string root) { root = root; }
        std::string &getRoot() { return root; }
        long long getClientMaxBodySize() { return static_cast<long long>(client_max_body_size); }
        std::vector<location> &getLocations() { return locations; }
        std::map <std::string, std::string> &getErrorPages() { return error_pages; }

        void setClienMaxBodySize(double n) { client_max_body_size = n; }

        Serv();
        ~Serv();
};


void parse_config(int ac, char **av);

