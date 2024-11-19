#include "webserv.hpp"
extern char **environ;

int CGI::execute(void)
{
	init();
	int _err_fd[2];

	if (pipe(_fd) == -1) // create pipe
		throw 500;
	if (pipe(_err_fd) == -1) // create pipe
		throw 500;
	if ((_cgi_child = fork()) == -1) // fork process
		throw 500;
	if (_cgi_child == 0) // child process
	{
		if (_req.get_method() == "POST")
		{
			cout << "POST body_path: " << _body_path.c_str() << endl;
			int fd = open(_body_path.c_str(), O_RDONLY);
			perror("open");
			if (fd == -1)
				throw 501;
			dup2(fd, STDIN_FILENO);
		}
		else
			close(STDIN_FILENO);
		if (dup2(_fd[1], STDOUT_FILENO) == -1) // redirect stdout to pipe
			throw 500;
		if (dup2(_err_fd[1], STDERR_FILENO) == -1) // redirect stderr to pipe
			throw 500;
		if (close(_fd[1]) || close(_fd[0])) // close write end and read end of pipe
			throw 500;
		if (close(_err_fd[1]) || close(_err_fd[0])) // close write end and read end of pipe
			throw 500;
		const char *argv[] = {_cgi_path.c_str(), _path.c_str(), NULL};
		execve(_cgi_path.c_str(), (char **)argv, environ);
		throw 501;
	}
	if (close(_fd[1]) || close(_err_fd[1])) // close write end of pipe
		throw 501;
	waitpid(_cgi_child, NULL, 0);
	while (1)
	{
		char buffer[1024];
		int bytes_read = read(_fd[0], buffer, 1024);
		if (bytes_read == -1)
			throw 501;
		if (bytes_read == 0)
			break;
		_output.append(buffer, bytes_read);
		if (bytes_read < 1024)
			break;
	}
	if (-1 == close(_fd[0])) // close read end of pipe
		throw 501;
	{
		char buffer[10];
		int bytes_read = read(_err_fd[0], buffer, 10);
		if (bytes_read == -1)
			throw 500;
		if (bytes_read)
			throw 501;
	}
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
	string _query_string = _req.get_URI().substr(get_CGI_script(_req.get_file_name(), _req.get_server_index(), 0).length());
	if (!_query_string.empty())
		_query_string = _query_string.substr(1);
	(
		setenv("REDIRECT_STATUS", "", 1) |
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
		setenv("QUERY_STRING", _query_string.c_str(), 1)
	) == -1 ? throw 500 : 0;
}

// Response*	CGI::get_response(void) // replace with above
string		CGI::get_response(void)
{
	string	output_file_path = generate_random_name();
	size_t	content_length = SIZE_MAX;
	bool	is_status_set = false;
	string	content;
	int boundary_len = (_output.find("\n\n") > _output.find("\r\n\r\n") ? 4: 2);
	size_t	heaers_end_pos =  (_output.find("\n\n") > _output.find("\r\n\r\n") ? _output.find("\r\n\r\n"): _output.find("\n\n"));
	ofstream file(output_file_path.c_str());
	if (heaers_end_pos == string::npos){
		content = _output;
	}
	else
	{
		string headers = _output.substr(0, heaers_end_pos);
		vector<string> header_lines = split_string_with_multiple_delemetres(headers, "\n");
		for (size_t i = 0; i < header_lines.size(); i++)
		{
			vector<string> header = split_string_with_multiple_delemetres(header_lines[i], ":");
			if (header.size() == 2)
			{
				if (header.at(0) == "Content-Length")
				{
					if (!is_status_set)
						file << "HTTP/1.1 200 OK\r\n";
					is_status_set = true;
					content_length = atoi(header.at(1).c_str());
				}
				else if (header.at(0) == "Status")
				{
					if (is_status_set)
						throw 500;
					file << "HTTP/1.1 " << header.at(1);
					is_status_set = true;
				}
				else
				{
					if (!is_status_set)
						file << "HTTP/1.1 200 OK\r\n";
					file << header_lines[i] << "\r\n"; // warning
					is_status_set = true;
				}
			}
		}
		if (content_length == SIZE_MAX || content_length  > _output.length() - heaers_end_pos)
			content = _output.substr(heaers_end_pos + boundary_len);
		else
			content = _output.substr(heaers_end_pos + boundary_len, content_length);
	}
	file << "Content-Length: " << content.length() << "\r\n";
	file << "\r\n";
	file << content;
	file.close();
	return (output_file_path);
}