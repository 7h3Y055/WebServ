#include "webserv.hpp"

void	delete_Response(Request *req)
{
	string source = servers[req->get_server_index()].getRoot() + get_location(req->get_file_name(), servers[req->get_server_index()]).getPath();

	if (access(source.c_str(), F_OK) == -1)
		throw 404;
	if (is_source_a_directory(source))
	{
		if (delete_directory(source.c_str()))
			throw 204; // No Content
	}
	else if (is_source_a_file(source))
		delete_file(source.c_str());
	
}
