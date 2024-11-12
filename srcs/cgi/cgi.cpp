#include "webserv.hpp"
extern char **environ;

int CGI::execute(void)
{
	init();

	if (access(_cgi_path.c_str(), F_OK) == -1) // if file does not exist
		return (-1);
	if (access(_cgi_path.c_str(), X_OK) == -1) // if file is not executable
		return (-2);
	if (pipe(_fd) == -1) // create pipe
		return (-1);
	if ((_stdout_copy = dup(1)) == -1) // save stdout
		return (-1);
	if (dup2(_fd[1], 1) == -1) // redirect stdout to pipe
		return (-1);
	if (close(_fd[1])) // close write end of pipe
		return (-1);
	if ((_cgi_child = fork()) == -1) // fork process
		return (-1);
	if (_cgi_child == 0) // child process
	{
		const char *argv[] = {_cgi_path.c_str(), _path.c_str(), NULL};
		execve(_cgi_path.c_str(), (char **)argv, environ);
	}
	waitpid(_cgi_child, NULL, 0);
	while (1)
	{
		char buffer[1024];
		int bytes_read = read(_fd[0], buffer, 1024);
		if (bytes_read == -1)
			return (-1);
		if (bytes_read == 0)
			break;
		_output.append(buffer, bytes_read);
		if (bytes_read < 1024)
			break;
	}
	close(_fd[0]); // close read end of pipe
	dup2(_stdout_copy, 1); // restore stdout
	close(_stdout_copy); // close copy of stdout
	cout << _output;


	cout << "\n\n=====================\n\n";
	return (0);
}

string	CGI::get_output(void)
{
	return (_output);
}

void	CGI::init(void)
{
	setenv("REQUEST_METHOD", _req.get_method().c_str(), 1);
	setenv("SCRIPT_NAME", _req.get_file_name().c_str(), 1);
	setenv("QUERY_STRING", _req.get_URI().substr(_req.get_file_name().length()).c_str(), 1);
	setenv("REMOTE_ADDR", _req.get_header("Host").c_str(), 1);
	setenv("CONTENT_LENGTH", _req.get_header("Content-Length").c_str(), 1);
	setenv("CONTENT_TYPE", _req.get_header("Content-Type").c_str(), 1);

	_body_path = _req.get_body_path();
	_filename = _req.get_file_name().c_str();
	_cgi_path = _loc.getCgi().at(_filename.substr(_filename.find_last_of('.')));
	_content_length = _req.get_fixed_length();
}