#include "webserv.hpp"

file_t    file_type(std::string source)
{
    struct stat sb;

    if (stat(source.c_str(), &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
            return DIRECTORY;
        else if (S_ISREG(sb.st_mode))
            return REGULAR_FILE;
        throw 403; // error_403
    }
    if (errno == EACCES)
        throw 403; // error_404
    if (errno == ENOENT)
        throw 404; // error_404
    throw 500; // error_500
}

bool	delete_directory(const char *path) {
    
    struct dirent	*entry;
	std::string		full_path;
    DIR				*dir = NULL;
	if ((dir = opendir(path)) == NULL)
    {
        if (errno == EACCES)
            throw 403; // error_404
        if (errno == ENOENT)
            throw 404; // error_404
        throw 500; // error_500
    }
    try
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;
            full_path = string(path) + "/" + entry->d_name;
            if (file_type(full_path) == DIRECTORY)
                if (delete_directory(full_path.c_str()) == false)
                    throw 403; // FORBIDDEN
            if (unlink(full_path.c_str()) == -1)
                    throw 403; // FORBIDDEN
        }
    }
    catch (int e)
    {
        if (closedir(dir) == -1)
            return false;
        throw e;
    }
    if (closedir(dir) == -1)
		return false;
	if (rmdir(path) == -1)
		throw 403; // FORBIDDEN
	return true;
}

void    delete_file(const char *path) {
	if (unlink(path) == -1)
		throw 403; // FORBIDDEN
	throw 204; // No Conten
}

string  to_string(ssize_t nbr)
{
    std::stringstream ss;
    ss << nbr;
    return ss.str();
}