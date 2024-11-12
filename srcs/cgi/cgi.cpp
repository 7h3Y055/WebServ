#include "webserv.hpp"
extern char **environ;

int CGI::execute(void)
{
	init();

	if (access(_cgi_path.c_str(), F_OK) == -1) // if file does not exist
		throw 404;
	if (access(_cgi_path.c_str(), X_OK) == -1) // if file is not executable
		throw 403;
	if (pipe(_fd) == -1) // create pipe
		throw 500;
	if ((_stdout_copy = dup(1)) == -1) // save stdout
		throw 500;
	if (dup2(_fd[1], 1) == -1) // redirect stdout to pipe
		throw 500;
	if (close(_fd[1])) // close write end of pipe
		throw 500;
	if ((_cgi_child = fork()) == -1) // fork process
		throw 500;
	if (_cgi_child == 0) // child process
	{
		const char *argv[] = {_cgi_path.c_str(), _path.c_str(), NULL};
		execve(_cgi_path.c_str(), (char **)argv, environ);
		throw 500;
	}
	waitpid(_cgi_child, NULL, 0);
	while (1)
	{
		char buffer[1024];
		int bytes_read = read(_fd[0], buffer, 1024);
		if (bytes_read == -1)
			throw 500;
		if (bytes_read == 0)
			break;
		_output.append(buffer, bytes_read);
		if (bytes_read < 1024)
			break;
	}
	if (-1 == close(_fd[0])) // close read end of pipe
		throw 500;
	if (-1 == dup2(_stdout_copy, 1)) // restore stdout
		throw 500;
	if (-1 == close(_stdout_copy)) // close copy of stdout
		throw 500;
	return (0);
}

void	CGI::init(void)
{
	(
		setenv("REQUEST_METHOD", _req.get_method().c_str(), 1) |
		setenv("SCRIPT_NAME", _req.get_file_name().c_str(), 1) |
		setenv("QUERY_STRING", _req.get_URI().substr(_req.get_file_name().length()).c_str(), 1) |
		setenv("REMOTE_ADDR", _req.get_header("Host").c_str(), 1) |
		setenv("CONTENT_LENGTH", _req.get_header("Content-Length").c_str(), 1) |
		setenv("CONTENT_TYPE", _req.get_header("Content-Type").c_str(), 1) |
		setenv("SERVER_PROTOCOL", _req.get_version().c_str(), 1) |
		setenv("SERVER_SOFTWARE", "webserv", 1) |
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 1) |
		setenv("PATH_INFO", _path.c_str(), 1)
	) == -1 ? throw 500 : 0;

	_body_path = _req.get_body_path();
	_filename = _req.get_file_name().c_str();
	_cgi_path = _loc.getCgi().at(_filename.substr(_filename.find_last_of('.')));
	_content_length = _req.get_fixed_length();
}

Response*	CGI::get_response(void)
{
	Response *res = new Response(_req);
	size_t	content_length = SIZE_MAX;
	string	content;
	size_t	heaers_end_pos = _output.find("\r\n\r\n");
	if (heaers_end_pos == string::npos)
		content = _output;
	else
	{
		string headers = _output.substr(0, heaers_end_pos);
		vector<string> header_lines = split_string_with_multiple_delemetres(headers, "\r\n");
		for (size_t i = 0; i < header_lines.size(); i++)
		{
			vector<string> header = split_string_with_multiple_delemetres(header_lines[i], ": ");
			if (header.size() == 2)
			{
				if (header[0] == "Content-Length")
					content_length = atoi(header[1].c_str());
				else
					res->set_header(header[0], header[1]);
			}
		}
	}
	if (content_length == SIZE_MAX || content_length  > _output.length())
		content = _output.substr(heaers_end_pos + 4);
	else
		content = _output.substr(heaers_end_pos + 4, content_length);
	res->set_status_code(200);
	res->set_status_message("OK");
	std::vector<char> body(content.begin(), content.end());
	res->set_body(body);
	return (res);
}