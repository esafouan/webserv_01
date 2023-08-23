#pragma once


#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <sstream>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "multi.hpp"
#include <cstdlib>
#include <stdio.h>


#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#define MAX_REQUEST_SIZE 2048

// typedef struct client_info
// {
//     socklen_t address_length;
//     struct sockaddr_storage address;
//     int socket;
//     char request[MAX_REQUEST_SIZE + 1];
//     int received;
//     struct client_info *next;
// }  s_client;

//mahdi

class Request 
{
    private :
        std::string method;
        std::string target;
        std::string httpVersion;
        std::string status;
        std::map <std::string, std::string> myRequest;
    
    public :
        Request(std::string req);
        ~Request();
};

///reda

class location
{
    public : 
        std::string NAME;
        std::string  root;
        bool autoindex;
        bool POST;
        bool GET;
        bool DELETE;
        std::string index;
        std::string _return;
        std::string alias;
        std::string cgi1;
        std::string cgi2;
};


class Server
{    
    public :
        std::vector<u_int16_t>  listen;
        std::string  host;
        std::string  server_name;
        std::map<int, std::string>error_page;
        int  max_body;
        std::string  root;
        std::string  index;
        std::vector<location>locations;
};

std::string removeSpaces(const std::string &input);
location *get_location(std::ifstream &Myfile, std::string &line);
std::vector<Server> mainf(int ac, char **av);
class args : std::exception
{
    public:
    const char * what() const throw()
    {
        return "configue file needed";        
    }
};
