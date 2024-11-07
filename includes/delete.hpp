#ifndef DELETE_HPP
#define DELETE_HPP
#include "webserv.hpp"
#include <time.h>
#include <iomanip>

using namespace std;

Response	*delete_Response(Request *req);
bool		is_source_a_directory(std::string source);
bool		is_source_a_file(std::string source);
bool		delete_directory(const char *path);
bool		delete_file(const char *path);
string		to_string(ssize_t nbr);
void		error_404(Response *res);
void		error_500(Response *res);
void		error_204(Response *res);
void		error_403(Response *res);

#endif