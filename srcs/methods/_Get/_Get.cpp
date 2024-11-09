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

Response &default_res(Request &req)
{
    Response *res = new Response(req);
    res->set_status_code(200);
    res->set_status_message("OK");
    res->set_header("Content-Type", "text/html");
    std::string content = "<html><body><h1>This is default response 🐧 </h1></body></html>";
    std::vector<char> body(content.begin(), content.end());
    res->set_body(body);
    return *res;
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
    int best_match = 0;
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

Response *get_Response(Request &req)
{
    std::string resources = req.get_URI();

    std::string root = servers[req.get_server_index()].getRoot();

    // std::cout << "this is root == " << root << std::endl;

    resources = root + resources;

    std::cout << "this is resources == " << resources << std::endl;

    if (access(resources.c_str(), F_OK) == -1)
    {
        Response *res = new Response(req);
        res->set_status_code(404);
        res->set_status_message("Not Found");
        res->set_header("Content-Type", "text/html");
        std::string content = "<html><body><h1>404 Not Found 🐧 </h1></body></html>";
        std::vector<char> body(content.begin(), content.end());
        res->set_body(body);
        return res;
    }
    if (get_resources_type(resources) == "directory")
    {
        Response *res = new Response(req);
        res->set_status_code(200);
        res->set_status_message("OK");
        res->set_header("Content-Type", "text/html");
        std::string content = "<html><body><h1>This is a directory 🐧 </h1></body></html>";
        std::vector<char> body(content.begin(), content.end());
        res->set_body(body);
        return res;
    }
    else
    {
        bool is_cgi = false;
        std::vector<location> locations = servers[req.get_server_index()].getLocations();
        location loc = find_best_location(locations, req.get_URI());
        if (loc.getCgi().size() > 0)
        {
            is_cgi = true;
        }

        std::cout << "this is resources " << resources << std::endl;


        

    }

    return &default_res(req);
}






/*



        for (int i = 0; i < loc.getCgi().size(); i++)
        {
            std::cout << "this is cgi == " << loc.getCgi()[i]["extension"] << std::endl;
        }


        std::cout << "this is location is cgi == " << is_cgi << std::endl;
        std::cout << "this is loc path == " << loc.getPath() << std::endl;

        for (size_t i = 0; i < loc.getMethods().size(); i++)
        {
            if (loc.getMethods()[i] == req.get_method())
            {
                Response *res = new Response(req);
                res->set_status_code(200);
                res->set_status_message("OK");
                res->set_header("Content-Type", fill_exts(resources.substr(resources.find_last_of('.'))));
                std::ifstream file(resources.c_str(), std::ios::binary);
                std::vector<char> body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                res->set_body(body);
                return res;
            }
            if (i == loc.getMethods().size() - 1 && loc.getMethods()[i] != req.get_method())
            {
                Response *res = new Response(req);
                res->set_status_code(405);
                res->set_status_message("Method Not Allowed");
                res->set_header("Content-Type", "text/html");
                std::string content = "<html><body><h1>405 Method Not Allowed 🐧 </h1></body></html>";
                std::vector<char> body(content.begin(), content.end());
                res->set_body(body);
                return res;
            }
        }



*/