#include "webserv.hpp"

extern	string  source;


int	run_cgi(void)
{
	int			fd[2];
	int			status;
	pid_t		cgi_child;
	int			stdout_copy;
	string		output;
	// Response	*res = new Response(*req);
	// string source = "/home/melfersi/cc/WebServ/cgi-bin/cgi"; //+ req->get_file_name();
	if (access(source.c_str(), F_OK) == -1) // if file does not exist
		return (-1);
	if (access(source.c_str(), X_OK) == -1) // if file is not executable
		return (-2);
	if (pipe(fd) == -1) // create pipe
		return (-1);
	if ((stdout_copy = dup(1)) == -1) // save stdout
		return (-1);
	if (dup2(fd[1], 1) == -1) // redirect stdout to pipe
		return (-1);
	if (close(fd[1])) // close write end of pipe
		return (-1);
	if ((cgi_child = fork()) == -1) // fork process
		return (-1);
	if (cgi_child == 0) // child process
	{
		char *argv[] = {NULL};
		char *envp[] = {NULL};
		execve(source.c_str(), argv, envp);
	}
	waitpid(cgi_child, NULL, 0);
	while (1)
	{
		char buffer[1024];
		int bytes_read = read(fd[0], buffer, 1024);
		if (bytes_read == -1)
			return (-1);
		if (bytes_read == 0)
			break;
		output.append(buffer, bytes_read);
		if (bytes_read < 1024)
			break;
	}
	cerr << output;
	close(fd[0]); // close read end of pipe
	dup2(stdout_copy, 1); // restore stdout
	close(stdout_copy); // close copy of stdout
	return (0);
}