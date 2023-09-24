#include "Request.hpp"

void Request::generate_error_page(Server &server)
{
    if (status != "200" && status != "301")
    {
        if(server.error_page.find(status) != server.error_page.end())
        {
            target = server.error_page[status];
        }
        else
            target = pages[status];
    }  
}

void Request::error_handling(Server &serv)
{
    if ((method == "GET" && serv.locations[0].GET == false) || (method == "POST" && serv.locations[0].POST == false) || (method == "DELETE" && serv.locations[0].DELETE == false))
        status = "405";
    
    else if (method != "GET" && method != "DELETE" && method != "POST")
    {
        if (method == "PUT" || method == "HEAD" || method == "TRACE" || method == "CONNECT")
            status = "501";
        else
            status = "404";
    }
    
    else if (target.size() > 2048)
        status = "414";
    
    else
    {
        int flag = 0;

        if (target == "/")
            status = replace_slash_in_target(serv, target, &flag);
        else if (target[0] == '/')
            target = target.substr(1);

        if (count_slash(target) == 0 && target != "/")
            short_uri(target, serv, &flag_uri);
        else if (count_slash(target) >= 1)
            long_uri(target, serv, &flag_uri);
    }

    if (find_key("Content-Length", StoreHeaders))
    {
        std::string val = valueOfkey("Content-Length", StoreHeaders);
        int content = std::atoi(val.c_str());

        if (content > serv.max_body)
            status = "413";
    }

    if (status == "200" && access(target.c_str(), F_OK) == -1)
    {
        status = "404";
    }
    else if (httpVersion != "HTTP/1.1")
        status = "400";
    
    else if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) != "chunked")

        status = "501";
    
    else if (!find_key("Transfer-Encoding", StoreHeaders) && !find_key("Content-Length", StoreHeaders) && method == "POST")
        status = "400";
    
    else if (find_key("Transfer-Encoding", StoreHeaders) && find_key("Content-Length", StoreHeaders) && method == "POST")
        status = "400";
}
