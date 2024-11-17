#include "webserv.hpp"
extern char **environ;

int CGI::execute(void)
{
	init();

	if (pipe(_fd) == -1) // create pipe
		throw 500;
	if ((_cgi_child = fork()) == -1) // fork process
		throw 500;
	if (_cgi_child == 0) // child process
	{
		if (dup2(_fd[1], STDOUT_FILENO) == -1) // redirect stdout to pipe
			throw 500;
		if (close(_fd[1]) || close(_fd[0])) // close write end and read end of pipe
			throw 500;
		if (_req.get_method() == "POST")
		{
			int fd = open(_body_path.c_str(), O_RDONLY);
			if (fd == -1)
				throw 500;
			dup2(fd, STDIN_FILENO);
		}
		const char *argv[] = {_cgi_path.c_str(), _path.c_str(), NULL};
		execve(_cgi_path.c_str(), (char **)argv, environ);
		throw 500;
	}
	if (close(_fd[1]))
		throw 500;
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
	return (0);
}

void	CGI::init(void)
{
	cout << _req.get_method() << endl;
	_body_path = _req.get_body_path();
	_path = _loc.getRoot() + get_CGI_script(_req.get_file_name(), _req.get_server_index(), 0);
	_filename = get_CGI_script(_req.get_file_name(), _req.get_server_index(), 0).c_str();
	_cgi_path = _loc.getCgi().at(_filename.substr(_filename.find_last_of('.')));
	_content_length = _req.get_fixed_length();
	cout << "_path :" << _path.c_str() << endl;
	(
		setenv("REDIRECT_STATUS", "", 1) |
		setenv("SERVER_SOFTWARE", "Webserv 42 (1337)", 1) |
		setenv("SERVER_NAME", "webserv", 1) |
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 1) |
		setenv("SERVER_PROTOCOL", "HTTP/1.1", 1) |
		setenv("SERVER_PORT", DEL::to_string(servers.at(_req.get_server_index()).getPort()).c_str(), 1) |
		setenv("REQUEST_METHOD", _req.get_method().c_str(), 1) |
		setenv("SCRIPT_FILENAME", _path.c_str(), 1) |
		setenv("PATH_TRANSLATED", _path.c_str(), 1) |
		setenv("PATH_INFO", _path.c_str(), 1) |
		setenv("SCRIPT_NAME", _path.c_str(), 1) |
		setenv("QUERY_STRING", _req.get_URI().substr(get_CGI_script(_req.get_file_name(), _req.get_server_index(), 0).length()).c_str(), 1)
	) == -1 ? throw 500 : 0;
	cout << "q:>>>>: " << _req.get_URI().substr(get_CGI_script(_req.get_file_name(), _req.get_server_index(), 0).length()).c_str() << endl;
}

/*


	1 - no more hardcoding for the http code .
	2 - the function should return a file .
	3 - the output should contain header .
	4 - check for the error in the cgi
	5 - query strings not working
	6 - post data (all the inputs not working ).
	7 - the output file should be in the client class !!!!!!!!!!!!
	8 - 


*/




Response*	CGI::get_response(void)
{
	Response *res = new Response(_req);
	size_t	content_length = SIZE_MAX;
	string	content;
	int boundary_len = (_output.find("\n\n") > _output.find("\r\n\r\n") ? 4: 2);
	size_t	heaers_end_pos =  (_output.find("\n\n") > _output.find("\r\n\r\n") ? _output.find("\r\n\r\n"): _output.find("\n\n"));
	if (heaers_end_pos == string::npos){
		content = _output;
	}
	else
	{
		string headers = _output.substr(0, heaers_end_pos);
		vector<string> header_lines = split_string_with_multiple_delemetres(headers, "\r\n");
		for (size_t i = 0; i < header_lines.size(); i++)
		{
			vector<string> header = split_string_with_multiple_delemetres(header_lines[i], ": ");
			if (header.size() == 2)
			{
				if (header.at(0) == "Content-Length")
					content_length = atoi(header.at(1).c_str());
				else
					res->set_header(header.at(0), header.at(1));
			}
		}
		if (content_length == SIZE_MAX || content_length  > _output.length() - heaers_end_pos)
			content = _output.substr(heaers_end_pos + boundary_len);
		else
			content = _output.substr(heaers_end_pos + boundary_len, content_length);
	}
	res->body_file_path_ref() = generate_random_name();
	ofstream file(res->body_file_path_ref().c_str());
	file << content;
	file.close();
	res->set_status_code(200);
	res->set_status_message("OK");
	std::vector<char> body(content.begin(), content.end());
	res->set_body(body);
	return (res);
}