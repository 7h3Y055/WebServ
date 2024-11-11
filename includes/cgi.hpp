#ifndef CGI_HPP
#define CGI_HPP



int	run_cgi(Request &req);


class CGI
{
	private:
		Request &_req;
		string	_filename;
		string	_cgi_path;
		string	_body_path;
		int		_fd[2];
		size_t	_content_length;
		int		_status;
		pid_t	_cgi_child;
		int		_stdout_copy;
		string	_output;
	public:
		void init(void);
		int	execute(void);
		Response	*parse_response(void);
		CGI(Request &req) : _req(req) {};
		~CGI() {};
};



#endif // CGI_HPP