#include "server.hpp"

Server::Server()
{
    this->host = "";
    this->root = "";
    this->index = "";
    int max_body = 0;
}

void Server::ft_split(std::string input, std::string delimiter, std::vector<std::string> &parts)
{
    size_t startPos = 0;
    size_t endPos;
    std::string tmp;

    while ((endPos = input.find(delimiter, startPos)) != std::string::npos)
    {
        tmp = input.substr(startPos, endPos - startPos);
        if(!tmp.empty())
            parts.push_back(tmp);
        startPos = endPos + delimiter.length();
    }

    // Add the last part
    tmp = input.substr(startPos);
    if(!tmp.empty())
            parts.push_back(tmp);
}

int check_digits(std::string hold)
{
    int i = 0;

    while (hold[i])
        if(!isdigit(hold[i++]))
            return 0;
    return 1;
}

int   Server::get_listen(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (!check_digits(hold[1]))
        return 0;
    if (hold[1].length() > 6)
        return 0;
    int listen = atoi(hold[1].c_str());

    if (listen > 65335 || listen < 0)
        return 0;
    
    SA_I s;

    server.server_sock.push_back(-1);
    memset(&s,0,sizeof(s));
    server.seraddr_s.push_back(s);
    server.listen.push_back((u_int16_t)listen);
    return(1);
}

int   Server::get_host(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    std::vector<std::string> host;

    Server::ft_split(hold[1], ".", host);
    if (host.size() != 4)
        return 0;
    int i = 0;
    while(i < host.size())
    {
        if (!check_digits(host[i]))
            return 0;
        else if (host[i].length() > 3)
            return 0;
        else if (atoi(host[i].c_str()) > 255 || atoi(host[i].c_str()) < 0)
            return 0;
        i++;
    }
    server.host = hold[1];
    return(1);
}

int   Server::get_server_name(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    server.server_name.push_back(hold[1]);
    return(1);
}

int   Server::get_max_body(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if(!check_digits(hold[1]))
        return 0;
    if (hold[1].length() > 10)
        return 0;
    int max = atoi(hold[1].c_str());
    if (max > 2000000000)
        return 0;
    server.max_body = max;
    return(1);
}

int   Server::get_root(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    server.root = hold[1];
    return(1);
}

int   Server::get_index(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    server.index = hold[1];
    return(1);
}
int Server::get_error_page(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 4)
        return 0;
    server.error_page.insert(std::pair<std::string, std::string>(hold[1], hold[3]));
    return (1);
}

Server::Server(char *config_file)
{
    std::ifstream c_file(config_file);
    // if(c_file)
    std::string line;    
    my_func pointer_to_fun[7] = {
        {"listen", &Server::get_listen},
        {"host", &Server::get_host},
        {"server_name", &Server::get_server_name},
        {"error_page", &Server::get_error_page},
        {"max_body", &Server::get_max_body},
        {"root", &Server::get_root},
        {"index", &Server::get_index}  
    };

    my_location ptr[7] = {
        {"root", &location::root_name},
        {"autoindex", &location::auto_index},
        {"POST", &location::post},
        {"GET", &location::get},
        {"DELETE", &location::deletee},
        {"index", &location::Index},
        {"return", &location::rreturn},
    };

    int j = 0;
    int location_flag ;
    while (std::getline(c_file, line))
    {
        if (line == "server{")
        {   
            Server serv;
            location_flag = 0;
            while (std::getline(c_file, line))
            {
                std::vector<std::string> holder;
                Server::ft_split(line, " ", holder);
                //int flag = 0;
                if (!location_flag)
                {
                    int flag = 0;
                    for (int i = 0; i < 7; i++)
                    {
                        if (pointer_to_fun[i].key == holder[0])
                        {
                            flag = 1;    
                            if (!(this->*(pointer_to_fun[i].my_function))(serv, holder))
                            {
                                std::cout << "0"<< std::endl;
                                throw error_config();
                            }
                        }
                    }
                    if (!flag)
                        location_flag = 1;
                }
                if (location_flag)
                {
                    if (holder[0] == "location")//location scop;
                    {
                        location loc;
                        loc.name(loc, holder);
                        
                        std::getline(c_file, line);
                        std::vector<std::string> holder;
                        Server::ft_split(line, " ", holder);
                        if (holder[0] != "{")
                        {
                            std::cout << "1"<< std::endl;
                            throw error_config();
                        }
                        while(std::getline(c_file, line))
                        {
                            std::vector<std::string> holder;
                            Server::ft_split(line, " ", holder);
                            if(holder.size() == 0)
                                throw error_config();
                            int flag = 0;
                            for (int i = 0; i < 7; i++)
                            {
                               // std::cout << holder[0]<< std::endl;
                                if (ptr[i].key_location == holder[0])
                                {
                                    flag = 1;
                                    if (!(loc.*(ptr[i].my_func))(loc, holder))
                                    {
                                        std::cout << "2" << std::endl;
                                        throw error_config();
                                    }
                                }    
                            }
                            if (!flag)
                            {
                                if (holder[0] != "}")
                                {
                                    throw error_config();
                                }
                                serv.locations.push_back(loc);
                                break;
                            }
                        }
                    }
                    else if (holder[0] == "};")
                    {
                        this->servers.push_back(serv);
                        break;
                    }
                    else
                        throw error_config();
                }
            }
        }
        else
        {
            std::cout << "5"<< std::endl;
            throw error_config();
        }
    }
        // for (int i = 0 ; i < servers.size(); i++)
        // {
        //     std::cout << "Server" << i <<"_______________________----------" <<std::endl;
        //     std::cout << "listen :"<< std::endl;
        //     std::vector<u_int16_t>::iterator iter = servers[i].listen.begin();
        //     for(iter; iter < servers[i].listen.end(); iter++)
        //         std::cout << *iter << " ";
        //     std::cout << std::endl;
        //     std::cout << "host : \n" << servers[i].host << std::endl;
        //     std::vector<std::string>::iterator it1 = servers[i].server_name.begin();
        //     for(it1; it1 < servers[i].server_name.end(); it1++)
        //         std::cout << "server_name :\n" << *it1 << std::endl;
        //     std::map<std::string , std::string>::iterator it = servers[i].error_page.begin();
        //     std::cout << "error_page :\n";
        //     while (it !=  servers[i].error_page.end())
        //     {
        //         std::cout << it->first << " :: " << it->second << std::endl;
        //         it++;
        //     }
        //     std::cout << "max_body :\n" << servers[i].max_body << std::endl;
        //     std::cout << "root :\n" << servers[i].root << std::endl;
        //     std::cout << "index :\n" << servers[i].index << std::endl;
        //     std::vector<location>::iterator itr = servers[i].locations.begin();
        //     for(itr; itr < servers[i].locations.end(); itr++)
        //     {
        //         std::cout << "loactions**************************** :\n";
        //         itr->print();
        //     }
        // }

}

Server::~Server(){
}