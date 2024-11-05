#include "webserv.hpp"
#include "utils.cpp"
#include "response_returns.cpp"
#ifndef _DELETE_RESPONSE_HPP
#define _DELETE_RESPONSE_HPP
using namespace std;

string root = "/home/melfersi/cc/WebServ/";

string path1 = "css";// dir
string path2 = "/css/folder"; // file


Response	*delete_Response(Request *req, string path)
{
	Response *res = new Response(*req);
	// get_request_source(req);
	string source = root + path;
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
#endif