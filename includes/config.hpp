#ifndef CONFIG_HPP
# define CONFIG_HPP
#include "ServerConfig.hpp"
#include <iostream>
#include <fstream>

void    config_parse(std::string file, vector<ServerConfig> &servers);

#endif // CONFIG_HPP
