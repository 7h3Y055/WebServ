#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP
#include <map>
#include <vector>

using namespace std;

class ServerConfig
{
private:
	map<string, string> 			__config;
	vector<map<string, string> >	__locations;
public:
	ServerConfig(map<string, string> config, vector<map<string, string> > location);
	string								get_config_value(string key) const;
	string								get_location_value(string key, unsigned int index) const;
	unsigned int						get_location_count() const;
	map<string, string>		const			&get_copy_location(unsigned int index) const;
};


#endif // SERVERCONFIG_HPP