#include "multi.hpp"


void err(std::string str)
{
    std::cout << str << std::endl;
    // _exit(1);
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
    if (ac > 2)
    {
        std::cout << "args" << std::endl;
        return 0;
    }
    
    epol ep;
    ep.ep_fd = epoll_create(1);
    if (ep.ep_fd == -1)
        err("epolllll");
    try
    {
        std::string a;
        if(ac == 1)
            a.append("webserve.conf");
        else
            a.append(av[1]);
        Server serv((char *)a.c_str());     
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