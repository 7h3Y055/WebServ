#include "webserv.hpp"
extern char **environ;

int CGI::get_cgi_output(path, loc, clients[client_fd]);
{
	init();
	// cout << _content_length << endl;
	// int			fd[2];
	// int			status;
	// pid_t		cgi_child;
	// int			stdout_copy;
	// string		output;
	// // Response	*res = new Response(*req);

	// cout << "_cgi_path: " << _cgi_path << endl;
	// if (access(_cgi_path.c_str(), F_OK) == -1) // if file does not exist
	// 	return (-1);
	// if (access(_cgi_path.c_str(), X_OK) == -1) // if file is not executable
	// 	return (-2);
	// if (pipe(fd) == -1) // create pipe
	// 	return (-1);
	// if ((stdout_copy = dup(1)) == -1) // save stdout
	// 	return (-1);
	// if (dup2(fd[1], 1) == -1) // redirect stdout to pipe
	// 	return (-1);
	// if (_req.get_method() == "POST")
	// {
	// 	int fd = open(_body_path.c_str(), O_RDONLY);
	// 	if (fd == -1)
	// 		return (-1);
	// 	char buffer[1024];
	// 	int bytes_read;
	// 	while ((bytes_read = read(fd, buffer, 1024)) > 0)
	// 		write(fd[1], buffer, bytes_read);
	// 	close(fd);
	// }
	// if (close(fd[1])) // close write end of pipe
	// 	return (-1);
	// if ((cgi_child = fork()) == -1) // fork process
	// 	return (-1);
	// if (cgi_child == 0) // child process
	// {
	// 	char *argv[] = {_cgi_path.c_str(), _filename.c_str(), NULL};
	// 	execve(_cgi_path.c_str(), argv, environ);
	// }
	// waitpid(cgi_child, NULL, 0);
	// while (1)
	// {
	// 	char buffer[1024];
	// 	int bytes_read = read(fd[0], buffer, 1024);
	// 	if (bytes_read == -1)
	// 		return (-1);
	// 	if (bytes_read == 0)
	// 		break;
	// 	output.append(buffer, bytes_read);
	// 	if (bytes_read < 1024)
	// 		break;
	// }
	// cerr << output;
	// close(fd[0]); // close read end of pipe
	// dup2(stdout_copy, 1); // restore stdout
	// close(stdout_copy); // close copy of stdout
	return (0);
}

void	CGI::init(void)
{
	setenv("REQUEST_METHOD", _req.get_method().c_str(), 1);
	setenv("SCRIPT_NAME", _req.get_file_name().c_str(), 1);
	setenv("QUERY_STRING", _req.get_URI().substr(_req.get_URI().find_first_of('?')).c_str(), 1);
	setenv("REMOTE_ADDR", _req.get_header("Host").c_str(), 1);
	setenv("CONTENT_LENGTH", _req.get_header("Content-Length").c_str(), 1);
	setenv("CONTENT_TYPE", _req.get_header("Content-Type").c_str(), 1);

	_body_path = _req.get_body_path();
	_filename = _req.get_file_name().c_str();
	_cgi_path = "cig_path";
	_content_length = _req.get_fixed_length();
}