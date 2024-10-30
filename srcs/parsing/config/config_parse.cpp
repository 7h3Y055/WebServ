#include "config.hpp"

using namespace std;

bool	config_parse(string file, vector<ServerConfig> &servers)
{
	vector<map<string, string> >	locations;
	map<string, string>				config;
	map<string, string>				location;
	ifstream						ifs(file);
	string							line;
	int								line_number = 0;
	line_t							type_line;

	while (getline(ifs, line))
	{
		line_number++;
		type_line = which_type(line);
		if (type_line == SERVER)
		{
			config.clear();
			locations.clear();
			while (getline(ifs ,line))
			{
				line_number++;
				type_line = which_type(line);
				if (type_line == PAIR)
				{
					if (!set_config(line, config))
						return (false);
				}
			}
		}	
	}
}


bool	set_config(string line, map<string, string> &config)
{
	
}