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
#include <dirent.h>

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#define MAX_REQUEST_SIZE 2048
#define LINE 4096

// typedef struct client_info
// {
//     socklen_t address_length;
//     struct sockaddr_storage address;
//     int socket;
//     char request[MAX_REQUEST_SIZE + 1];
//     int received;
//     struct client_info *next;
// }  s_client;



/// reda

class location
{
public:
    std::string NAME;
    std::string root;
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
public:
    std::vector<u_int16_t> listen;
    std::string host;
    std::string server_name;
    std::map<int, std::string> error_page;
    int max_body;
    std::string root;
    std::string index;
    std::vector<location> locations;
};

// mahdi

class Request 
{
   public :
        std::string method;
        std::string target;
        std::string httpVersion;
        std::string status;
        int _fd;
        std::map <std::string, std::string> myRequest;
    
    public :
        Request(std::string req, int fd, Server server);
        void print_element();
        void error_handling(Server &serv);
        ~Request();
};

class soc
{
    private:
        struct sockaddr_in seraddr, client_addr;
    private:
        std::string ip;
        u_int16_t port;
        int socket_fd;
        Server ser;
        std::vector<Request> req;
    private:
        void fill_ser_Add(sockaddr_in *add)
        {
            add->sin_family = AF_INET;
            add->sin_addr.s_addr = htonl(INADDR_ANY);
            add->sin_port = htons(port);
        }
    private:
        int ep_fd;
        struct epoll_event ev;
        struct epoll_event events[1];
    private:
        int request_part(int fd_cl);
    private:
        void response(int fd_cl);
    public:
        soc() {}
        soc(Server s) : ser(s)
        {
            ip = s.host;
            port = s.listen[0];
        }
    void init();
    void run();
};
void err(std::string str);
std::string removeSpaces(const std::string &input);
location *get_location(std::ifstream &Myfile, std::string &line);
std::vector<Server> mainf(int ac, char **av);

std::string constructResponseHeader(const std::string &contentType, size_t contentLength, std::vector<Request> req);
std::string birng_content(std::vector<Request> req, int reciver);
std::string generateDirectoryListing(const std::string &directoryPath);
class args : std::exception
{
public:
    const char *what() const throw()
    {
        return "configue file needed";
    }
};
