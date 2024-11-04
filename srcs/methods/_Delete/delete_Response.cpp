#include "webserv.hpp"


using namespace std;

string root = "/home/melfersi/cc/WebServ/www";

Response	&delete_Response(Request *req)
{
	Response *res = new Response(*req);
	string resource = req->get_file_name();

	return *res;
}