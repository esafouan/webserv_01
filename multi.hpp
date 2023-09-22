#pragma once

#include "config_file/server.hpp"

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
        int still_reading;
        std::string uri_for_response;
        int flag_uri;
        std::ifstream infile;
        std::string infile_name;
        int epol;
        std::string boundary_separater;

        std::string extension;
        //post
        int endOfrequest;
        std::vector<std::pair<std::string, std::string> > StoreHeaders;
        std::vector<std::string> Body;
        int post_flag;
        std::ofstream ostrea;
        std::string Post_status;
        int get_flag;
        std::string cgi_filename;
        std::string cgi_body;
        std::string time;
        std::string query;

        std::string content_type;
        std::string content_lenght;
        std::string accept;
    public :
        static int num_file;
        Request(std::string req, Server server);
        Request(Request const &req);
        Request();
        Request& operator=(Request const & req);
        void print_element();
        void error_handling(Server &serv);
        ~Request();
        void creating_file(std::vector<std::pair<std::string, std::string> > &postReq, std::string &bod);
        void get_post_status();
        void Delete_methode();
        void get_target_page();
        void cgi_information();
};


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
