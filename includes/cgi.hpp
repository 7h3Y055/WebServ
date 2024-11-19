#ifndef CGI_HPP
#define CGI_HPP


#include <string>
using namespace std;


int	run_cgi(Request &req);


class CGI
{
	private:
		string		_path;
		location&	_loc;
		Request&	_req;
		string		_filename;
		string		_cgi_path;
		string		_body_path;
		int			_fd[2];
		size_t		_content_length;
		int			_status;
		pid_t		_cgi_child;
		int			_stdout_copy;
		int			_stderr_copy;
		int			_stdin_copy;
		string		_output;
	public:
		void		init(void);
		int			execute(void);
		string		get_response(void);
		CGI(Request &req, location &loc) 
		: _req(req) , _loc(loc) {};
		~CGI() {};
};



#endif // CGI_HPP