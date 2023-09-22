#include "multi.hpp"


void err(std::string str)
{
    std::cout << str << std::endl;
    // _exit(1);
}
std::string constructResponseHeader(const std::string &contentType, std::string status)
{
    std::string header;

    
    // Status line
    header += "HTTP/1.1 ";
    header += status;
    header += "\r\n";

    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    //chunks
    header += "Transfer-Encoding: chunked\r\n";

    // Blank line
    header += "\r\n";

    return header;
}

std::string get_content_type(const char *path)
{
    const char *last_dot = strrchr(path, '.');
    if (last_dot)
    {
        if (strcmp(last_dot, ".css") == 0)
            return "text/css";
        if (strcmp(last_dot, ".mp4") == 0)
            return "video/mp4";
        if (strcmp(last_dot, ".csv") == 0)
            return "text/csv";
        if (strcmp(last_dot, ".gif") == 0)
            return "image/gif";
        if (strcmp(last_dot, ".htm") == 0)
            return "text/html";
        if (strcmp(last_dot, ".html") == 0)
            return "text/html";
        if (strcmp(last_dot, ".ico") == 0)
            return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".py") == 0)
            return "application/python";
        if (strcmp(last_dot, ".pyon") == 0)
            return "application/json";
        if (strcmp(last_dot, ".png") == 0)
            return "image/png";
        if (strcmp(last_dot, ".pdf") == 0)
            return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0)
            return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0)
            return "text/plain";
        if (strcmp(last_dot, ".php") == 0)
            return "application/x-httpd-php";
    }
    return "";
}

// std::string birng_content(std::vector<Request> req, int reciver)
// {
//     std::string target;
//     for (int i = 0; i < req.size(); i++)
//         if (req[i].fd == reciver)
//             target = req[i].target;
//     target = target.substr(1);
//     if (target == "")
//         return "";
//     target = get_content_type(target.c_str());

//     return target;
// }

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "configue file needed" << std::endl;
        return 0;
    }
    epol ep;
    ep.ep_fd = epoll_create(1);
    if (ep.ep_fd == -1)
        err("epolllll");
    try
    {
        Server serv(av[1]);     
        for(int i = 0;i < serv.servers.size();i++)
        {
            init(serv.servers[i],&ep);
        }
        run(serv.servers,&ep);
    }
    catch(std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    

    
}