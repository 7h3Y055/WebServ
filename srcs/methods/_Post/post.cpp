#include "webserv.hpp"

location get_location(std::string file_name, Serv config)
{
    location loc;
    int loc_n = 0;
    for (size_t i = 0; i < config.getLocations().size(); i++)
    {
        if (strncmp(file_name.c_str(), config.getLocations()[i].getPath().c_str(), config.getLocations()[i].getPath().size()) == 0)
        {
            if (config.getLocations()[i].getPath().size() > loc_n)
            {
                loc_n = config.getLocations()[i].getPath().size();
                loc = config.getLocations()[i];
            }
        }
    }
    return loc;
}

std::string generate_random_name()
{
    std::string name;
    std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    while (true)
    {
        for (size_t n = 0; n < 8; n++)
            name += base[rand() % base.size()];
        std::ifstream file(name.c_str());
        if (!file.is_open())
            return name;
    }
    
}



Response *post_Response(Request &req){
    Response *res = new Response(req);
    location loc = get_location(req.get_file_name(), servers[req.get_server_index()]);

    try
    {
        if (find(loc.getMethods().begin(), loc.getMethods().end(), "POST") != loc.getMethods().end())
        {
            if (loc.getUploadPath().size() != 0){
                std::string path = servers[req.get_server_index()].getRoot() + "/" + loc.getUploadPath();
                path = path + "/" + generate_random_name() + get_extention(req);
                std::cout << path << std::endl;
                std::ofstream file(path.c_str(), std::ios::binary);
                if (!file.is_open())
                    throw 500;
                file.write(&req.get_body()[0], req.get_body().size());
                file.close();
                throw 201;
            }
            else
                throw 101;
        }
        else
            throw 405;
    }
    catch(const int& code)
    {
        return createResponse(code, req);
    }
    
    return res;
}


