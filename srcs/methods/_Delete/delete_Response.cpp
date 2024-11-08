#include "webserv.hpp"

void	delete_Response(Request *req)
{
	string source = servers[req->get_server_index()].getRoot() + req->get_file_name();

	cout << "source: " << source << endl;
	if (access(source.c_str(), F_OK) == -1)
		throw 404;
	cout << "source: " << source << endl;
	if (is_source_a_directory(source))
	{
		cout << "source: " << source << endl;
		cout << "delete_Response: source is a directory" << endl;
		if (delete_directory(source.c_str()))
			throw 204; // No Content
	}
	else if (is_source_a_file(source))
		delete_file(source.c_str());
}
