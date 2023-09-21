#include <ctime>
#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <dirent.h>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <unistd.h>
#include <cerrno>
#include <sys/stat.h>

class location
{
    private :
        std::string NAME ;
        std::string  root ;    
        std::string index ;
        std::string _return ;  
        bool POST;
        bool autoindex;
        bool GET;
        bool DELETE;

    public :
        location();
        int post(location &location, std::vector<std::string> &hold);
        int get(location &location, std::vector<std::string> &hold);
        int auto_index(location &location, std::vector<std::string> &hold);
        int deletee(location &location, std::vector<std::string> &hold);
        int name(location &location, std::vector<std::string> &hold);
        int rreturn(location &location, std::vector<std::string> &hold);
        int root_name(location &location, std::vector<std::string> &hold);
        int Index(location &location, std::vector<std::string> &hold);
        void    print()
        {
            std::cout << NAME << std::endl;
            std::cout << "root :\n" << root << std::endl;
            std::cout << "autoindex :\n" << autoindex << std::endl;
            std::cout << "POST :\n" << POST << std::endl;
            std::cout << "GET :\n" << GET << std::endl;
            std::cout << "DELETE :\n" << DELETE << std::endl;
            std::cout << "index :\n" << index << std::endl;
            std::cout << "return :\n" << _return << std::endl;
        }
        ~location();
};

location::location()
{
    NAME = "";
    root = "";    
    index = "";
    _return = "";  
}

int location::post(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.POST = true;
    else if (hold[1] == "off")
        location.POST = false;
    else
        return 0;
    return 1;
}

int location::get(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.GET = true;
    else if (hold[1] == "off")
        location.GET = false;
    else
        return 0;
    return 1;
}

int location::auto_index(location &location, std::vector<std::string> &hold)
{
     if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.autoindex = true;
    else if (hold[1] == "off")
        location.autoindex = false;
    else
        return 0;
    return 1;
}

int location::deletee(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.DELETE = true;
    else if (hold[1] == "off")
        location.DELETE = false;
    else
        return 0;
    return 1;
}

int location::name(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location.NAME = hold[1];
    return 1;
}

int location::rreturn(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location._return = hold[1];
    return 1;
}

int location::root_name(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location.root = hold[1];
    return 1;
}

int location::Index(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location.index = hold[1];
    return 1;
} 

location::~location()
{}

/////////////////////////SERVEEEEER///////////////////////////////

class Server
{    
    private :
        std::vector<u_int16_t>  listen;
        std::string  host;
        std::vector <std::string>  server_name;
        std::map <std::string, std::string> error_page;
        int  max_body;
        std::string  root;
        std::string  index;
        std::vector <location> locations;
        std::vector<int > fd_sock; //client
        std::vector<int > server_sock;
        //std::vector<SA_I> seraddr_s;
        std::vector <Server> servers;
        struct my_func
        {
            std::string key;
            int (Server::*my_function)(Server &server, std::vector<std::string> &hold);
        };    

        struct my_location
        {
            std::string key_location;
            int (location::*my_func)(location &location, std::vector<std::string> &hold);
        };
    public:
        Server();
        Server(char *config_file);
        ~Server();
        int    get_listen(Server &server, std::vector<std::string> &hold);
        int    get_error_page(Server &server, std::vector<std::string> &hold);
        int    get_host(Server &server, std::vector<std::string> &hold);
        int    get_server_name(Server &server, std::vector<std::string> &hold);
        int    get_max_body(Server &server, std::vector<std::string> &hold);
        int    get_root(Server &server, std::vector<std::string> &hold);
        int    get_index(Server &server, std::vector<std::string> &hold);
};

class error_config : public std::exception
{
    public:
    const char *what() const throw()
    {
        return("Error in config file !");
    }
};

Server::Server()
{
    this->host = "";
    this->root = "";
    this->index = "";
    int max_body = 0;
}

void ft_split(std::string input, std::string delimiter, std::vector<std::string> &parts)
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

int   Server::get_listen(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    server.listen.push_back((u_int16_t)atoi(hold[1].c_str()));
    return(1);
}

int   Server::get_host(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
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
    server.max_body = atoi(hold[1].c_str());
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

Server::Server(char *config_file) : Server()
{
    std::ifstream c_file(config_file);
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
                ft_split(line, " ", holder);
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
                        ft_split(line, " ", holder);
                        if (holder[0] != "{")
                        {
                            std::cout << "1"<< std::endl;
                            throw error_config();
                        }
                        while(std::getline(c_file, line))
                        {
                            std::vector<std::string> holder;
                            ft_split(line, " ", holder);
                            int flag = 0;
                            for (int i = 0; i < 7; i++)
                            {
                                if (ptr[i].key_location == holder[0])
                                {
                                    flag = 1;
                                    if (!(loc.*(ptr[i].my_func))(loc, holder))
                                    {
                                        std::cout << "2"<< std::endl;
                                        throw error_config();
                                    }
                                }    
                            }
                            if (!flag)
                            {
                                if (holder[0] != "}")
                                {
                                    std::cout << holder[0]<< std::endl;
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
                    {
                        std::cout << holder[0] << std::endl;
                        throw error_config();
                    }
                }
            }
        }
        else
        {
            std::cout << "5"<< std::endl;
            throw error_config();
        }
    }
        for (int i = 0 ; i < servers.size(); i++)
        {
            std::cout << "Server" << i <<"_______________________----------" <<std::endl;
            std::cout << "listen :"<< std::endl;
            std::vector<u_int16_t>::iterator iter = servers[i].listen.begin();
            for(iter; iter < servers[i].listen.end(); iter++)
                std::cout << *iter << " ";
            std::cout << std::endl;
            std::cout << "host : \n" << servers[i].host << std::endl;
            std::vector<std::string>::iterator it1 = servers[i].server_name.begin();
            for(it1; it1 < servers[i].server_name.end(); it1++)
                std::cout << "server_name :\n" << *it1 << std::endl;
            std::map<std::string , std::string>::iterator it = servers[i].error_page.begin();
            std::cout << "error_page :\n";
            while (it !=  servers[i].error_page.end())
            {
                std::cout << it->first << " :: " << it->second << std::endl;
                it++;
            }
            std::cout << "max_body :\n" << servers[i].max_body << std::endl;
            std::cout << "root :\n" << servers[i].root << std::endl;
            std::cout << "index :\n" << servers[i].index << std::endl;
            std::vector<location>::iterator itr = servers[i].locations.begin();
            for(itr; itr < servers[i].locations.end(); itr++)
            {
                std::cout << "loactions**************************** :\n";
                itr->print();
            }
        }

}

Server::~Server(){
}

int main(int ac, char **av)
{
    try
    {
        Server a(av[1]);
    }
    catch(std::exception &conf)
    {
        std::cout << conf.what() << std::endl;
    }

    return 0;
}