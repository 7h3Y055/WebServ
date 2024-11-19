#pragma once

#include "webserv.hpp"

Response *get_Response(Request &req);
location find_best_location(std::vector<location> locations, std::string uri);
std::vector<std::string> get_directory_content(std::string path);
std::string get_resources_type(std::string resources);
std::string get_mime_type(std::string key);
Response &default_res(Request &req);
bool is_it_a_cgi(std::string path);
std::string  fill_exts(std::string key);
std::vector<char> generate_header(std::ifstream &file, std::string path);
std::string _to_string(int n);