#include "webserv.hpp"

Response post_Response(Request &req){
    Response res = Response(req);

    try
    {
        throw 404;
    }
    catch(const int& code)
    {
        return createResponse(code, req);
    }
    
    
    
    
    return res;
}

