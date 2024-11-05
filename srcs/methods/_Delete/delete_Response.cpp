#include "webserv.hpp"

Response	*delete_Response(Request *req)
{
	Response *res = new Response(*req);
	// get_request_source(req);
	// string source = root + path1;
	string source = req->get_file_name();
	cout << source << endl;
	if (access(source.c_str(), F_OK) == -1)
	{
		error_404(res);
		return res;
	}
	if (is_source_a_directory(source))
	{
		delete_directory(source.c_str()) ? error_204(res) : error_500(res);
		return res;
	}
	if (is_source_a_file(source))
	{
		delete_file(source.c_str()) ? error_204(res) : error_500(res);
		return res;
	}
	return res;
}
