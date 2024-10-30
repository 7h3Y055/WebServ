#include "config.hpp"

// #include <cctype>

string	trim(string line)
{
	if (line.empty())
		return (line);
	size_t start = line.find_first_not_of(" \t");
	size_t end = line.find_last_not_of(" \t");
	return (line.substr(start, end - start + 1));
}

bool	is_valid_pair(string line)
{
	size_t pos = line.find('=');
	if (pos == string::npos)
		return (false);
	string key = trim(line.substr(0, pos));
	string value = trim(line.substr(pos + 1));
	cout << "key = \"" << key << "\"" << endl;
	cout << "value = \"" << value << "\"" << endl;
	if (key.empty() || value.empty())
		return (false);
	if (!isalpha(key[0]) && key[0] != '_')
		return (false);
	for (size_t i = 0; i < key.length(); i++)
		if (!isalnum(key[i]) && key[i] != '_')
			return (false);
	return (true);
}

line_t	which_type(string line)
{
	line.erase(line.find_first_of("#"));
	line = trim(line);
	if (line == "SERVER::")
		return (SERVER);
	if (line == "\tlocation:")
		return (LOCATION);
	if (line.empty())
		return (EMPTY);
	return (is_valid_pair(line) ? PAIR : SYNTAX_ERROR);
}