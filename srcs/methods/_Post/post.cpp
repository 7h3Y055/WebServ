#include "webserv.hpp"

location get_location(std::string file_name, Serv &config)
{
    location loc;
    size_t loc_n = 0;
    for (size_t i = 0; i < config.getLocations().size(); i++)
    {
        if (file_name.find(config.getLocations()[i].getPath()) != string::npos)
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

int my_rand()
{
    char c;
    std::ifstream random("/dev/urandom");
    random.read(&c, 1);
    random.close();
    return c;
}

std::string generate_random_name()
{
    std::string name; 
    std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

    while (true){
        name = "/tmp/";
        for (size_t n = 0; n < 64; n++)
            name += base[my_rand() % base.size()];
        if (access(name.c_str(), F_OK) == -1){
            return name;
        }
    }
    return name;
}


void move(std::string src, std::string dst){
    ofstream out(dst.c_str(), std::ios::binary);
    if (src.size() == 0){
        out.close();
        return ;
    }
    ifstream in(src.c_str());
    if (!in.is_open() || !out.is_open())
        throw 500;
    
    in.seekg(0, std::ios::end);
    if (in.tellg() == 0){
        in.close();
        throw 204;
    }
    in.seekg(0, std::ios::beg);

    out << in.rdbuf();
    in.close();
    out.close();
    if (remove(src.c_str()))
        throw 500;
}


Response *Request::post_Response(){
    Response *res = new Response(*this);
    location loc = get_location(get_file_name(), servers[get_server_index()]);
    string location_header;
    if (find(loc.getMethods().begin(), loc.getMethods().end(), "POST") != loc.getMethods().end())
    {
        if (loc.getUploadPath().size() != 0){
            if (_Body_path.size() < 1)
                throw 204;

            std::string path = loc.getRoot() + "/" + loc.getUploadPath();
            path = path + "/" + _Body_path.substr(5) + get_extention(*this);
            location_header = loc.getUploadPath() + "/" + _Body_path.substr(5) + get_extention(*this);
            move(_Body_path, path);
        }
        else
            throw 403;
    }
    else
        throw 405;

    res->set_status_code(201);
    res->set_status_message("Created");
    res->set_header("Location", location_header);
    vector<char> body;
    res->set_body(body);
    return res;
}


