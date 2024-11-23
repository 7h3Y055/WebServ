#ifndef CGI_HPP
#define CGI_HPP


#define STILL_RUNNING -1
#include <string>
using namespace std;


class Client;
class CGI
{
	private:
		string		_path;
		Request&	_req;
		location&	_loc;
		string		_filename;
		string		_cgi_path;
		string		_body_path;
		string		_ouput_file_path;
		size_t		_content_length;
		int			_status;
		pid_t		_cgi_child;
		string		_output;
		int			_in_file_fd;
		int			_out_file_fd;
		int			_err_file_fd;
	public:
		void		init(void);
		int			execute(Client *cli);
		string		get_response(void);
		CGI(Request &req, location &loc) 
		: _req(req) , _loc(loc) {};
		~CGI() {};
};



#endif // CGI_HPP