#include "ServerConfig.hpp"
#include <fstream>
#include <stdexcept>


 
ServerConfig::ServerConfig(map<string, string> config, vector<map<string, string> > locations)
	: __config(config), __locations(locations)
{
}

string		ServerConfig::get_config_value(string key) const
{
	return (this->__config.at(key));
}

string		ServerConfig::get_location_value(string key, unsigned int index) const
{
	return (this->__locations.at(index).at(key));
}

unsigned int	ServerConfig::get_location_count() const
{
	return (this->__locations.size());
}

map<string, string> const &ServerConfig::get_copy_location(unsigned int index) const
{
	return this->__locations.at(index);
}
