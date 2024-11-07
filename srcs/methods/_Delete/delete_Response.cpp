#include "webserv.hpp"

Response	*delete_Response(Request *req)
{
	Response *res = new Response(*req);
	// get_request_source(req);

	string source = req->get_file_name();
	cout << source << endl;
	if (access(source.c_str(), F_OK) == -1)
		return error_404(res), res;
	if (is_source_a_directory(source))
		return delete_directory(source.c_str()) ? error_204(res) : error_500(res), res;
	if (is_source_a_file(source))
		return delete_file(source.c_str()) ? error_204(res) : error_403(res), res;
	return res;
}
