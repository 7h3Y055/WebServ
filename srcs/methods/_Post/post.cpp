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
    std::string name = "temp_"; //HERE
    std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    while (true)
    {
        name = "temp_"; // HERE
        for (size_t n = 0; n < 8; n++)
            name += base[rand() % base.size()];
        std::ifstream file(name.c_str());
        if (!file.is_open())
            return name;
    }
    
}


int move(std::string src, std::string dst){
    ifstream in(src.c_str());
    ofstream out(dst.c_str(), std::ios::binary);

    if (!in.is_open() || !out.is_open())
        return 0;
    out << in.rdbuf();
    in.close();
    out.close();
    if (remove(src.c_str()))
        return 1;
    return 0;
}


Response *Request::post_Response(){
    Response *res = new Response(*this);
    location loc = get_location(get_file_name(), servers[get_server_index()]);

    try
    {
        if (find(loc.getMethods().begin(), loc.getMethods().end(), "POST") != loc.getMethods().end())
        {
            if (loc.getUploadPath().size() != 0){
                std::string path = servers[this->get_server_index()].getRoot() + "/" + loc.getUploadPath();
                path = path + "/" + generate_random_name() + get_extention(*this);
                std::cout << _Body_path << " ==> " << path << std::endl;
                if (move(_Body_path, path) != 0)
                    throw 500;
                // std::ofstream file(path.c_str(), std::ios::binary);
                // if (!file.is_open())
                    // throw 500;
                // file.write(&this->get_body()[0], this->get_body().size());
                // file.close();
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
        return createResponse(code, this);
    }

    return createResponse(201, this);
    return res;
}


