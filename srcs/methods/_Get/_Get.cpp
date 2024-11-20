#include "../includes/_Get.hpp"


std::string  fill_exts(std::string key)
{
    std::map<std::string, std::string> mime_types;
    mime_types.insert(std::make_pair(".txt", "text/plain"));
    mime_types.insert(std::make_pair(".html", "text/html"));
    mime_types.insert(std::make_pair(".css", "text/css"));
    mime_types.insert(std::make_pair(".js", "text/javascript"));
    mime_types.insert(std::make_pair(".gif", "image/gif"));
    mime_types.insert(std::make_pair(".bmp", "image/bmp"));
    mime_types.insert(std::make_pair(".webp", "image/webp"));
    mime_types.insert(std::make_pair(".wav", "audio/wav"));
    mime_types.insert(std::make_pair(".ogg", "audio/ogg"));
    mime_types.insert(std::make_pair(".webm", "video/webm"));
    mime_types.insert(std::make_pair(".ogv", "video/ogg"));
    mime_types.insert(std::make_pair(".swf", "application/x-shockwave-flash"));
    mime_types.insert(std::make_pair(".epub", "application/epub+zip"));
    mime_types.insert(std::make_pair(".ttf", "application/x-font-ttf"));
    mime_types.insert(std::make_pair(".woff", "application/x-font-woff"));
    mime_types.insert(std::make_pair(".woff2", "application/x-font-woff2"));
    mime_types.insert(std::make_pair(".otf", "application/x-font-otf"));
    mime_types.insert(std::make_pair(".eot", "application/x-font-eot"));
    mime_types.insert(std::make_pair(".bin", "application/octet-stream"));
    mime_types.insert(std::make_pair(".svg", "application/x-font-svg"));
    mime_types.insert(std::make_pair(".json", "application/json"));
    mime_types.insert(std::make_pair(".xml", "application/xml"));
    mime_types.insert(std::make_pair(".jpg", "image/jpeg"));
    mime_types.insert(std::make_pair(".png", "image/png"));
    mime_types.insert(std::make_pair(".pdf", "application/pdf"));
    mime_types.insert(std::make_pair(".zip", "application/zip"));
    mime_types.insert(std::make_pair(".mp3", "audio/mpeg"));
    mime_types.insert(std::make_pair(".mp4", "video/mp4"));
    mime_types.insert(std::make_pair(".doc", "application/msword"));
    mime_types.insert(std::make_pair(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
    mime_types.insert(std::make_pair(".xls", "application/vnd.ms-excel"));
    mime_types.insert(std::make_pair(".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
    mime_types.insert(std::make_pair(".ppt", "application/vnd.ms-powerpoint"));
    mime_types.insert(std::make_pair(".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
    mime_types.insert(std::make_pair(".rar", "application/x-rar-compressed"));
    mime_types.insert(std::make_pair(".tar", "application/x-tar"));
    mime_types.insert(std::make_pair(".mkv", "video/x-matroska"));

    return mime_types[key];
}

std::string get_resources_type(std::string resources)
{
    struct stat s;
    if (stat(resources.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
            return "directory";
        else if (s.st_mode & S_IFREG)
            return "file";
    }
    return "none";
}


location find_best_location(std::vector<location> locations, std::string uri)
{
    location best_location;
    size_t best_match = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        std::string path = locations[i].getPath();
        if (uri.find(path) == 0 && path.size() > best_match)
        {
            best_match = path.size();
            best_location = locations[i];
        }
    }
    return best_location;
}

std::vector<std::string> get_directory_content(std::string path)
{
    std::vector<std::string> directory_content;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string file_name = ent->d_name;
            if (file_name != "." && file_name != "..")
            {
                directory_content.push_back(file_name);
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "could not open directory" << std::endl;
        throw 403;
    }
    return directory_content;
}

std::vector<char> generate_header(std::ifstream &file, std::string path)
{
    std::vector<char> header;
    std::string status_line = "HTTP/1.1 200 OK\r\n";
    header.insert(header.end(), status_line.begin(), status_line.end());
    std::string content_type = "Content-Type: ";
    size_t pos = path.find_last_of('.');
    std::string key;
    if(pos != std::string::npos)
        key = fill_exts(path.substr(pos));
    else
        key = "text/plain";
    content_type += key + "\r\n";
    header.insert(header.end(), content_type.begin(), content_type.end());
    std::string content_length = "Content-Length: ";
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);
    content_length += _to_string(length) + "\r\n";
    header.insert(header.end(), content_length.begin(), content_length.end());
    header.push_back('\r');
    header.push_back('\n');
    return header;
}

