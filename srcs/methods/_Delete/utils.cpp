#include "webserv.hpp"

bool    is_source_a_directory(std::string source)
{
    struct stat sb;
    if (stat(source.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
}

bool    is_source_a_file(std::string source)
{
    struct stat sb;
    if (stat(source.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
        return true;
    return false;
}

bool	delete_directory(const char *path) {
    struct dirent	*entry;
	std::string		full_path;
    DIR				*dir = NULL;

	if ((dir = opendir(path)) == NULL) 
		return false;
    while ((entry = readdir(dir)) != NULL)
	{
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        full_path = string(path) + "/" + entry->d_name;
        if (is_source_a_directory(full_path))
            if (delete_directory(full_path.c_str()) == false) return false;
		if (is_source_a_file(full_path))
			if (unlink(full_path.c_str()) == -1) return false;
    }
    if (closedir(dir) == -1)
		return false;
	if (rmdir(path) == -1)
		return false;
	return true;
}

bool	delete_file(const char *path) {
	if (unlink(path) == -1)
		return false;
	return true;
}

string  to_string(ssize_t nbr)
{
    std::stringstream ss;
    ss << nbr;
    return ss.str();
}
