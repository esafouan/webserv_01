#pragma once

#include "Request/Request.hpp"

#define MAX_REQUEST_SIZE 2048
#define LINE 4096



std::string valueOfkey(std::string key, std::vector<std::pair<std::string, std::string> > &postR);

typedef struct ep
{
    int  ep_fd;
    struct epoll_event ev;
    struct epoll_event events[1];
} epol ;

std::string construct_res_dir_list(const std::string &contentType, size_t contentLength);
int chunked_response(std::string target, int client_fd, std::map<int, Request> &req);
int directorie_list(std::string target, int client_fd, std::map<int, Request> &req);
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
std::string generateDirectoryListing(const std::string &directoryPath,  std::map<int, Request> &req, int client_fd);
std::string constructResponseHeader(const std::string &contentType, std::string status);
std::string birng_content(std::vector<Request> req, int reciver);
std::string opendir(const std::string &directoryPath, std::map<int, Request> &req, int client_fd);
std::string get_content_type(const char *path);
std::string construct_error_page(const std::string &contentType, size_t contentLength,std::string status);
class args : std::exception

{
public:
    const char *what() const throw()
    {
        return "configue file needed";
    }
};
