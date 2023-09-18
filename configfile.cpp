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
#include "multi.hpp"
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

struct location
{
    std::string NAME = "";
    std::string  root = "";    
    std::string index = "";
    std::string _return = "";  
    bool POST;
    bool autoindex;
    bool GET;
    bool DELETE;
};

class Server
{    
    private :
        std::vector<u_int16_t>  listen;
        std::string  host;
        std::vector <std::string>  server_name;
        std::map <int, std::string> error_page;
        int  max_body;
        std::string  root;
        std::string  index;
        std::vector <location> locations;
        std::vector<int > fd_sock; //client
        std::vector<int > server_sock;
        std::vector<SA_I> seraddr_s;
        struct my_func
        {
            std::string key;
            void (Server::*my_function)(Server &server, std::string value);
        };
    public:
        Server();
        Server(char *config_file);
        ~Server();
        void    get_listen(Server &server, std::string value);
        void    get_host(Server &server, std::string value);
        void    get_server_name(Server &server, std::string value);
        void    get_error_page(Server &server, std::string value);
        void    get_max_body(Server &server, std::string value);
        void    get_root(Server &server, std::string value);
        void    get_index(Server &server, std::string value);
};

Server::Server(){
    this->host = "";
    this->root = "";
    this->index = "";
    int max_body = 0;

}

std::string removeSpaces(const std::string &input) 
{
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\t'), result.end());

    return result;
}
void ft_split(std::string input, std::string delimiter, std::vector<std::string> &parts)
{
    size_t startPos = 0;
    size_t endPos;
    std::string tmp;
    input = removeSpaces(input);
    while ((endPos = input.find_first_of(delimiter, startPos)) != std::string::npos)
    {
        tmp = input.substr(startPos, endPos - startPos);
        if(!tmp.empty())
            parts.push_back(tmp);
        startPos = endPos + delimiter.length();
    }
    // Add the last part
    tmp = input.substr(startPos);
    if (!tmp.empty())
        parts.push_back(tmp);
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

    while (std::getline(c_file, line))
    {
        std::vector<std::string> holder;
        ft_split(line, " \t\n", holder);

    }
}

Server::~Server(){
}

int main()
{
}