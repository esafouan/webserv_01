#include "server.hpp"


Server::Server()
{
    this->host = "";
    this->root = "";
    this->index = "";
    this->server_name = "";
    this->upload_path = "";
    this->max_body = -1;
    this->listen = -1;
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

    if (listen > 65535 || listen < 1024)
        return 0;
    
    SA_I s;

    server.server_sock = -1;
    memset(&s,0,sizeof(s));
    server.seraddr_s = s;
    server.listen = (u_int16_t)listen;
    server.duplicate_in_server.push_back("listen");
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
    while(i < (int)host.size())
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
    server.duplicate_in_server.push_back("host");
    return(1);
}

int   Server::get_server_name(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    server.duplicate_in_server.push_back("server_name");
    server.server_name = hold[1];
    return(1);
}

int   Server::get_max_body(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if(!check_digits(hold[1]))
      return 0;
    std::istringstream stream(hold[1]);
    long long num;
    if (stream >> num)
    {}
    else {
        return 0;
    }
    server.max_body = num;
    server.duplicate_in_server.push_back("max");

    return(1);
}

int   Server::get_root(Server &server, std::vector<std::string> &hold)
{  
    if (hold.size() != 2)
        return 0;
    if (access(hold[1].c_str(), F_OK) == -1)
        return 0;
    if(hold[1][hold[1].length() - 1] != '/')
        return (0);
    server.root = hold[1];
    server.duplicate_in_server.push_back("root");
    return (1);
}

int   Server::get_index(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    server.index = hold[1];
    server.duplicate_in_server.push_back("index");
    return(1);
}

int Server::get_error_page(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 4)
        return 0;
    server.error_page.insert(std::pair<std::string, std::string>(hold[1], hold[3]));
    return (1);
}

int Server::Upload(Server &server, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1][hold[1].length() - 1] != '/')
        return (0);
    if (access(hold[1].c_str(), F_OK) == -1)
        return 0;
    server.upload_path = hold[1];
    server.duplicate_in_server.push_back("upload_path");

    return 1;
}

int Server::check_duplicate(Server &server)
{
    std::map<std::string, std::string> tmp;

    for (int i = 0; i < (int)server.duplicate_in_server.size(); i++)
    {
        if (tmp.count(server.duplicate_in_server[i]) == 1)
            return 0;
        tmp[server.duplicate_in_server[i]] = "";
    }
    if (tmp.count("listen") == 0 || tmp.count("host") == 0 || tmp.count("server_name") == 0)
        return 0;
    return 1;
}

void    Server::fill_server( Server &serv ,my_func *pointer_to_fun)
{
    std::vector<std::string> holder;
    Server::ft_split(line, " ", holder);

    if (!location_flag)
    {
        int flag = 0;
        for (int i = 0; i < 8; i++)
        {
            if (holder.size() > 0 && pointer_to_fun[i].key == holder[0])
            {
                flag = 1;
                if (!(this->*(pointer_to_fun[i].my_function))(serv, holder))
                    throw error_config();
            }
        }

        if (!flag)
        {      
            if(!check_duplicate(serv))
                throw error_config();
            location_flag = 1;
        }
           
    }
}

 int    Server::fill_locations(std::ifstream &c_file, Server &serv , my_location *ptr)
 {

    if (location_flag)
    {  
        std::vector<std::string> holder;
        Server::ft_split(line, " ", holder);

        if (holder.size() > 0 && holder[0] == "location")//location scop;
        {
            location loc;
            
            if(!loc.name(loc, holder))
                throw error_config();
            std::getline(c_file, line);
            std::vector<std::string> holder;
            Server::ft_split(line, " ", holder);
            if (holder.size() > 0 && holder[0] != "{")
                throw error_config();
            while(std::getline(c_file, line))
            {
                std::vector<std::string> holder;
                Server::ft_split(line, " ", holder);
                if(holder.size() == 0)
                    throw error_config();
                int flag = 0;
                for (int i = 0; i < 10; i++)
                {
                    if (holder.size() > 0 && ptr[i].key_location == holder[0])
                    {
                        flag = 1;
                        if (!(loc.*(ptr[i].my_func))(loc, holder))
                            throw error_config();
                    }    
                }
                if (!flag)
                {
                    if (holder.size() > 0 && holder[0] != "}")
                        throw error_config();
                    if (!loc.loc_duplicate())
                        throw error_config();
                    serv.locations.push_back(loc);
                    break;
                }
            }
        }
        else if (holder.size() > 0 && holder[0] == "};")
        {
            this->servers.push_back(serv);
            return 1;
        }
        else
            throw error_config();
    }
    return 0;
}
Server::Server(char *config_file)
{
    std::ifstream c_file(config_file);
    if (!c_file.good())
        throw error_config();

    my_func pointer_to_fun[8] = {
        {"listen", &Server::get_listen},
        {"host", &Server::get_host},
        {"server_name", &Server::get_server_name},
        {"error_page", &Server::get_error_page},
        {"max_body", &Server::get_max_body},
        {"root", &Server::get_root},
        {"upload_path", &Server::Upload},
        {"index", &Server::get_index}  
    };

    my_location ptr[10] = {
        {"root", &location::root_name},
        {"autoindex", &location::auto_index},
        {"POST", &location::post},
        {"GET", &location::get},
        {"DELETE", &location::deletee},
        {"index", &location::Index},
        {"return", &location::rreturn},
        {"cgi", &location::cgi_state},
        {"upload", &location::upload_state},
        {"upload_path", &location::uploadpath},
    };

    while (std::getline(c_file, line))
    {
        if (line == "server{")
        {   
            Server serv;
            location_flag = 0;
            while (std::getline(c_file, line))
            {
                if (!location_flag)
                    fill_server( serv ,pointer_to_fun);
                if (location_flag)
                    if (fill_locations(c_file, serv , ptr))
                        break;
            }
        }
        else
            throw error_config();
    }
    std::map<std::string , std::string> check;
    for (int i = 0 ; i < (int)servers.size(); i++)
    {
        std::stringstream stream;
        stream << servers[i].listen;
        
        std::string dst = "";
        dst += servers[i].host + stream.str()+ servers[i].server_name;
        if (check.count(dst) == 1)
            throw error_config();
        check[dst] = "";
    }


}

Server::~Server()
{
}