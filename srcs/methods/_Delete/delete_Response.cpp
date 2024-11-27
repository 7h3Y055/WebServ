#include "webserv.hpp"

void	delete_Response(Request *req)
{
	string source = servers[req->get_server_index()].getRoot() + req->get_file_name();

	cout << "\033[1;36m[WebServ]\033[0m " << "source: " << source << endl;
	if (access(source.c_str(), F_OK) == -1)
	{
		if (errno == EACCES)
			throw 403;
		if (errno == ENOENT)
			throw 404;
		throw 500;
	}
	if (file_type(source) == DIRECTORY)
	{
		if (source[source.length() - 1] != '/')
			throw 409; // Conflict
		if (delete_directory(source.c_str()))
			throw 204; // No Content
	}
	delete_file(source.c_str());
}