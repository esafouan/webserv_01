#pragma once
#include "location.hpp"
#define SA_I struct sockaddr_in

class location;
class Server
{    
    public:
        u_int16_t listen;
        std::string  host;
        std::string server_name;
        std::map <std::string, std::string> error_page;
        long long  max_body;
        std::string  root;
        std::string  index;
        std::vector <location> locations;

        std::vector<int > fd_sock; //client
        int  server_sock;
        SA_I seraddr_s;
        
        std::vector <Server> servers;
        int location_flag;
        std::string line; 
        std::string upload_path;
        std::vector<std::string> duplicate_in_server;
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
        int     check_duplicate(Server &server);
        void    fill_server( Server &serv ,my_func *pointer_to_fun);
        int     fill_locations(std::ifstream &c_file, Server &serv , my_location *ptr);
        void    ft_split(std::string input, std::string delimiter, std::vector<std::string> &parts);
        int     get_listen(Server &server, std::vector<std::string> &hold);
        int     get_error_page(Server &server, std::vector<std::string> &hold);
        int     get_host(Server &server, std::vector<std::string> &hold);
        int     get_server_name(Server &server, std::vector<std::string> &hold);
        int     get_max_body(Server &server, std::vector<std::string> &hold);
        int     get_root(Server &server, std::vector<std::string> &hold);
        int     get_index(Server &server, std::vector<std::string> &hold);
        int     Upload(Server &server, std::vector<std::string> &hold);
};

class error_config : public std::exception
{
    public:
    const char *what() const throw()
    {
        return("Error in config file !");
    }
};
std::string get_current_time();
