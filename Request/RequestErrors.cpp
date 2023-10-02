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

void Request:: error_handling(Server &serv)
{
    if (method != "GET" && method != "DELETE" && method != "POST")
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
        if (target == "/")
            replace_slash_in_target(serv);

        else if (count_slash(target) == 1 && target != "/")
            short_uri(serv);

        else if (count_slash(target) > 1)
            long_uri(serv);
    }
   
    if (status == "200")
    {   
        if (access(target.c_str(), F_OK ) == -1)
        {
            status = "404";
        }
        else if (find_key("Content-Length", StoreHeaders))
        {
            std::string val = valueOfkey("Content-Length", StoreHeaders);
            int content = std::atoi(val.c_str());
            if (content > serv.max_body)
                status = "413";
        }
        else if (method == "GET"  && access(target.c_str(), R_OK) == -1)
        {
             std::cout << "3333"<<std::endl;
             status = "403";
        }
           
        else if (target.find(".php") != target.npos || target.find(".py") != target.npos)
        {
            if (access(target.c_str(), X_OK) == -1)
            {
                std::cerr <<"tar = " <<target <<std::endl;
                status = "403";
            }
               
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

}
