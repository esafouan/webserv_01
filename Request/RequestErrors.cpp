#include "Request.hpp"

void Request::generate_error_page(Server &server)
{
    if (status != "200" && status != "301")
    {
        if (server.error_page.find(status) != server.error_page.end())
            target = server.error_page[status];
        else
            target = pages[status];
    }  
}

void Request:: error_handling(Server &serv)
{
    if (serv.max_body > 0)
        maxbody = serv.max_body;
    if (method != "GET" && method != "DELETE" && method != "POST")
        status = "501";
    else if (target.size() > 2048)
        status = "414";
    else
    {
        if (target == "/")
            replace_slash_in_target(serv);
        if (count_slash(target) == 1 && target != "/")
            short_uri(serv);
        if (count_slash(target) > 1)
            long_uri(serv);
    }
    if (access(target.c_str(), F_OK) == -1)
        encoded_uri();
    if (status == "200")
    {   
        real_path();
        if (access(target.c_str(), F_OK ) == -1)
            status = "404";
        else if (find_key("Content-Length", StoreHeaders))
        {
            std::stringstream stream(valueOfkey("Content-Length", StoreHeaders));
            stream >> this->lenght_of_content;
            if (serv.max_body > 0)
            {
                if (this->lenght_of_content > serv.max_body)
                    status = "413";
            }
        }
        else if (method == "GET"  && access(target.c_str(), R_OK) == -1)
            status = "403";
           
        else if (target.find(".php") != target.npos || target.find(".py") != target.npos)
        {
            if (access(target.c_str(), X_OK) == -1)
                status = "403"; 
        }

        if (httpVersion != "HTTP/1.1")
            status = "400";
        
        else if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) != "chunked")
            status = "501";
        
        else if (!find_key("Transfer-Encoding", StoreHeaders) && !find_key("Content-Length", StoreHeaders) && method == "POST")
            status = "400";
        
        // else if (find_key("Transfer-Encoding", StoreHeaders) && find_key("Content-Length", StoreHeaders) && method == "POST")
        //     status = "400";
    }
}
