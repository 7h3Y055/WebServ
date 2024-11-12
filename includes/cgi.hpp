#ifndef CGI_HPP
#define CGI_HPP



int	run_cgi(Request &req);


class CGI
{
	private:
		string&		_path;
		location&	_loc;
		Client&		_clients;
		Request&	_req;
		string		_filename;
		string		_cgi_path;
		string		_body_path;
		int			_fd[2];
		size_t		_content_length;
		int			_status;
		pid_t		_cgi_child;
		int			_stdout_copy;
		string		_output;
	public:
		void		init(void);
		int			execute(void);
		Response	*get_response(void);
		CGI(Request &req, string &path, location &loc, Client &client) 
		: _req(req) , _path(path), _loc(loc), _clients(client) {};
		~CGI() {};
};



#endif // CGI_HPP