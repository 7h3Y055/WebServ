
#ifndef DELETE_HPP
#define DELETE_HPP
#include "webserv.hpp"
#include <time.h>
#include <iomanip>

using namespace std;

typedef enum
{
	REGULAR_FILE,
	DIRECTORY,
}	file_t;

void		delete_Response(Request *req);
file_t		file_type(std::string source);
bool		delete_directory(const char *path);
void		delete_file(const char *path);
string		to_string(ssize_t nbr);

#endif
