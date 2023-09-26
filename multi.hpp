#pragma once

#include "Request/Request.hpp"

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

// class Request 
// {
//    public :
//         int child_exited;
//         int header_for_cgi_resp;
//         clock_t time_of_child;
//         int is_forked_before;
//         int pipefd[2];
//         pid_t pid_of_the_child;
//         std::string method;
//         std::string target;
//         std::string httpVersion;
//         std::string status;
//         int header_flag;
//         std::ifstream fd_file;
//         int lenght_Readed;
//         int lenght_of_content;
//         int calcul_chunk_flag;
//         int cgi_flag;
//         size_t chunk_size;
//         size_t Bytes_readed;
//         std::string rest_of_boundry;
//         int open_boundry_file;
//         std::ofstream outfile;
//         std::string outfile_name;
//         std::string rest_of_buffer;
//         int still_reading;
//         std::string uri_for_response;
//         int flag_uri;
//         std::ifstream infile;
//         std::string infile_name;
//         int epol;
//         std::string boundary_separater;
//         size_t chunked_file_size_response;
//         std::string extension;
//         //post
//         int endOfrequest;
//         std::vector<std::pair<std::string, std::string> > StoreHeaders;
//         std::vector<std::string> Body;
//         int post_flag;
//         std::ofstream ostrea;
//         std::string Post_status;
//         int get_flag;
//         std::string cgi_filename;
//         std::string cgi_body;
//         std::string time;
//         std::string query;

//         std::string content_type;
//         std::string content_lenght;
//         std::string accept;

//         std::map<std::string, std::string>pages;
//         std::map<std::string, std::string> extensions;
//     public :
//         static int num_file;
//         Request(std::string req, Server server);
//         Request(Request const &req);
//         Request();
//         Request& operator=(Request const & req);
//         void print_element();
//         void error_handling(Server &serv);
       
//         void creating_file(std::vector<std::pair<std::string, std::string> > &postReq, std::string &bod);
//         void get_post_status();
//         void Delete_methode();
//         void fill_error_pages_map();
//         void cgi_information(); 
//         void fill_extensions_map();

//         ~Request();
// };
class Response
{
    private : 
        int client_fd;
        std::map<int, Request> &req;
    public:
        Response(int f,std::map<int, Request> &r);
  
        int get_client_fd();
        
        bool is_get();
       
        bool is_cgi();
        
        bool is_delete();
       
        bool is_error();
        
        bool is_redirection();
        
        bool is_post();
      
        std::string normal_pages_header(size_t contentLength);
       
        std::string redirect_pages_header();
        
        void response_by_a_page(std::string path);
       
        std::string get_content_type();
       
        int directorie_list();
        
        std::string chunked_header();
       
        int chunked_response_headers();
        
        int chunked_response_body();
        std::string normal_pages_header1(size_t contentLength);
        std::string generateDirectoryListing();
        std::string get_last();
       
};

std::string valueOfkey(std::string key, std::vector<std::pair<std::string, std::string> > &postR);

typedef struct ep
{
    int  ep_fd;
    struct epoll_event ev;
    struct epoll_event events[10];
} epol ;



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
std::string opendir(const std::string &directoryPath, std::map<int, Request> &req, int client_fd);
std::string get_content_type(const char *path);

class args : std::exception

{
public:
    const char *what() const throw()
    {
        return "configue file needed";
    }
};
