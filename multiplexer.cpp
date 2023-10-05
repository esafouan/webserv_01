#include "multi.hpp"

int main(int ac, char **av)
{
    if (ac > 2)
    {
        std::cerr << "args" << std::endl;
        return 0;
    }
    
    epol ep;
    ep.ep_fd = epoll_create(1);
    if (ep.ep_fd == -1)
        perror("epolllll");
    try
    {
        std::string a;
        if(ac == 1)
            a.append("webserve.conf");
        else
            a.append(av[1]);
        Server serv((char *)a.c_str());  
        std::vector<std::pair<std::string,u_int16_t> >hosts;
        for(int i = 0;i < (int)serv.servers.size();i++)
        {
            int flag = 0;
            for(int j = 0;j < (int)hosts.size();j++)
            {
                if(hosts[j].first == serv.servers[i].host)
                    if(hosts[j].second == serv.servers[i].listen )
                    {
                        flag = 1;
                        break ;
                    }
            }
            if(!flag)
            {
                init(serv.servers[i],&ep);
                hosts.push_back(std::pair<std::string ,u_int16_t>(serv.servers[i].host,serv.servers[i].listen));
            }
            
        }
        run(serv.servers,&ep);
    }
    catch(std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    

    
}