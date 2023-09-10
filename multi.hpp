#pragma once

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

#define SA_I struct sockaddr_in

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
        std::map<std::string, std::string> cgi;
        location()
        {
            NAME = "";
            root = "";
            autoindex = 0;
            POST = 0;
            GET = 0;
            DELETE = 0;
            index = "";
            _return = "";
            alias = "";
        }
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
            std::cout << "alias :\n" << alias << std::endl;
        }
};


class Server
{    
    public :
        std::vector<u_int16_t>  listen;
        std::string  host;
        std::vector<std::string>  server_name;
        std::map<int, std::string>error_page;
        int  max_body;
        std::string  root;
        std::string  index;
        std::vector<location>locations;
        std::vector<int >fd_sock;//client
        std::vector<int > server_sock;
        std::vector<SA_I> seraddr_s;

        Server()
        {
            host = "";
            max_body = -1;
            root = "";
            index = "";
        }
        void print()
        {
            std::cout << "listen :"<< std::endl;
            std::vector<u_int16_t>::iterator iter = listen.begin();
            for(iter; iter < listen.end(); iter++)
                std::cout << *iter << " ";
            std::cout << std::endl;
            std::cout << "host : \n" << host << std::endl;
            std::vector<std::string>::iterator it1 = server_name.begin();
            for(it1; it1 < server_name.end(); it1++)
                std::cout << "server_name :\n" << *it1 << std::endl;
            std::map<int , std::string>::iterator it = error_page.begin();
            std::cout << "error_page :\n";
	        while (it != error_page.end())
	        {
                std::cout << it->first << " :: " << it->second << std::endl;
                it++;
            }
            std::cout << "max_body :\n" << max_body << std::endl;
            std::cout << "root :\n" << root << std::endl;
            std::cout << "index :\n" << index << std::endl;
            std::vector<location>::iterator itr = locations.begin();
            for(itr; itr < locations.end(); itr++)
            {
                std::cout << "loactions**************************** :\n";
                itr->print();
            }
        }
};

// mahdi

class Request 
{
   public :
        std::string method;
        std::string target;
        std::string httpVersion;
        std::string status;
        int header_flag;
        int fd_file;
        int lenght_Readed;
        int lenght_of_content;
        int calcul_chunk_flag;
        size_t chunk_size;
        size_t Bytes_readed;
        std::string rest_of_boundry;
        int open_boundry_file;
        std::ofstream outfile;
        std::string outfile_name;
        std::string rest_of_buffer;


        std::ifstream infile;
        std::string infile_name;

        std::string boundary_separater;

        std::string extension;
        //post
        int endOfrequest;
        std::vector<std::pair<std::string, std::string> > StoreHeaders;
        std::vector<std::string> Body;
        int post_flag;
        std::ofstream ostrea;
        std::string Post_status;
    public :
        Request(std::string req, Server server);
        Request(Request const &req);
        Request();
        Request& operator=(Request const & req);
        void print_element();
        void error_handling(Server &serv);
        ~Request();
        void creating_file(std::vector<std::pair<std::string, std::string> > &postReq, std::string &bod);
        void get_post_status();
};

// class soc
// {
//     private:
//         struct sockaddr_in seraddr, client_addr;
//     private:
//         std::string ip;
//         u_int16_t port;
//         int socket_fd;
//         Server ser;
//         std::vector<Request> req;
//     private:
//         void fill_ser_Add(sockaddr_in *add)
//         {
//             add->sin_family = AF_INET;
//             add->sin_addr.s_addr = htonl(INADDR_ANY);
//             add->sin_port = htons(port);
//         }
//     private:
//         int ep_fd;
//         struct epoll_event ev;
//         struct epoll_event events[1];
//     private:
//         int request_part(int fd_cl);
//     private:
//         void response(int fd_cl);
//     public:
//         soc() {}
//         soc(Server s) : ser(s)
//         {
//             ip = s.host;
//             port = s.listen[0];
//         }
//     
// };

std::string valueOfkey(std::string key, std::vector<std::pair<std::string, std::string> > &postR);

typedef struct ep
{
    int  ep_fd;
    struct epoll_event ev;
    struct epoll_event events[1];
} epol ;
std::string get_current_time();
std::string construct_res_dir_list(const std::string &contentType, size_t contentLength);
int chunked_response(std::string target, int client_fd, std::map<int, Request> &req);
int directorie_list(std::string target, int client_fd);
int response_header(std::string target, int client_fd, std::map<int, Request> &req);
void err(std::string str);
void init(Server& ser,epol *ep);
void fill_ser_Add(Server &ser,int i);
void run( std::vector<Server> servers,epol *ep);
void fill_ser_Add(Server &ser,int i);
void response();

std::string removeSpaces(const std::string &input);
location *get_location(std::ifstream &Myfile, std::string &line);
std::vector<Server> mainf(int ac, char **av);

std::string constructResponseHeader(const std::string &contentType, std::string status);
std::string birng_content(std::vector<Request> req, int reciver);
std::string generateDirectoryListing(const std::string &directoryPath);
std::string get_content_type(const char *path);
class args : std::exception

{
public:
    const char *what() const throw()
    {
        return "configue file needed";
    }
};
