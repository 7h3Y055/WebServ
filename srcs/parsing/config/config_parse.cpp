#include "config.hpp"

using namespace std;

void    config_parse(string file, vector<ServerConfig> &servers)
{
	vector<map<string, string> >	locations;
	map<string, string>				config;
	ifstream						ifs(file);
	string							line;
	int								line_number = 1;

	if (!ifs.is_open())
		throw runtime_error("Failed to open file: " + file + " !!");
	while (getline(ifs, line))
	{
		if (line.empty() || line[0] == '#')
			continue;
		if (line.find("[SERVER] {") != string::npos)
		{	
			config.clear();
			locations.clear();
			while (getline(ifs, line))
			{
				if (line.empty() || line[0] == '#')
					continue;
				if (line.at(0) == '}')
					break;
				if (line.find("location:") != string::npos)
				{
					LOCATION:
					map<string, string> location;
					while (getline(ifs, line))
					{
						if (line.empty() || line[0] == '#')
							continue;
						if (line.find("}") != string::npos)
							break;					/*check '=' exist because      here it's underfined behavior if .find() return string::nopos*/
						location[line.substr(0, line.find("="))] = (line.find("=") != string::npos) ? line.substr(line.find("=") + 1) : "";
						line_number++;
					}
					locations.push_back(location);
				}
				else													/* same check take a look above */
					config[line.substr(0, line.find("="))] = (line.find("=") != string::npos) ? line.substr(line.find("=") + 1) : "";
				line_number++;
			}
			servers.push_back(ServerConfig(config, locations));		
		}
		else
			throw runtime_error("Syntax error in file: " + file + " at line: " + to_string(line_number) + " !!");
		line_number++;
	}
}
