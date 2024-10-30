#ifndef CONFIG_HPP
# define CONFIG_HPP
#include "ServerConfig.hpp"
#include <iostream>
#include <fstream>

typedef enum line_e
{
	SERVER,
	LOCATION,
	PAIR,
	COMMANT,
	EMPTY,
	SYNTAX_ERROR
}	line_t;

bool    config_parse(std::string file, vector<ServerConfig> &servers);
line_t	which_type(std::string line);

#endif // CONFIG_HPP
