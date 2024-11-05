#include "webserv.hpp"
#include "utils.cpp"

string content = "<html><body><h1>DELETE</h1></body></html>";

vector<char> body(content.begin(), content.end());




void	error_404(Response *res)
{
		res->set_status_code(404);
		res->set_status_message("Not Found");
		res->set_header("Content-Length", to_string(body.size()));
		res->set_header("Content-Type", "text/html");
		res->set_header("Server", "webserv/29.16666666729");
		res->set_body(body);
}

void	error_500(Response *res)
{
		res->set_status_code(500);
		res->set_status_message("Internal Server Error");
		res->set_header("Content-Length", to_string(body.size()));
		res->set_header("Content-Type", "text/html");
		res->set_header("Server", "webserv/29.16666666729");
		res->set_body(body);
}

void	error_204(Response *res)
{
		res->set_status_code(204);
		res->set_status_message("No Content");
		res->set_header("Content-Length", to_string(body.size()));
		res->set_header("Content-Type", "text/html");
		res->set_header("Server", "webserv/29.16666666729");
		res->set_body(body);
}

