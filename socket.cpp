#include "multi.hpp"

#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
int Request::num_file = 0;


std::string get_current_time()
{
    std::time_t currentTime;
    std::time(&currentTime);
    char timeString[100]; 
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", std::localtime(&currentTime));
    std::string time(timeString);

    return (time);
}

size_t get_chunk_size(std::string str)
{
    size_t pos;
    pos = str.find("\r\n");
    char *status;
    size_t chunk_size = std::strtol(str.substr(0, pos).c_str(), &status, 16);

    return chunk_size;
}

void remove_size_of_chunk(std::string &str)
{
    str = str.substr(str.find("\n") + 1);
}

std::string get_file_extension(std::string &str,int client_fd, std::map<int, Request> &req)
{
    std::string ext;
    size_t pos;
    try
    {
        pos = str.find("Content-Disposition: ");
        str = str.substr(pos);
        pos = str.find("\r\n"); 
        str = str.substr(pos + 2);

        std::string tmp;
        pos = str.find("\n");
        tmp  = str.substr(0, pos);
        if(tmp.find("Content-Type: ") != tmp.npos)
        {
            pos = str.find("Content-Type: ");
            if(pos != str.npos)
            {
                str = str.substr(pos);
                pos = str.find("\n");
                ext = str.substr(14,pos - 15);
                pos = str.find("\r\n\r\n"); 
                str = str.substr(pos + 4);
                return (req[client_fd].extensions[ext]);
            }
        } 
        str = str.substr(2); 
    }
    catch(...)
    {}
    return(".txt");
}

void write_content(int client_fd, std::map<int, Request> &req, int close, std::string &content, int size)
{
    req[client_fd].outfile.write(content.c_str(), size);
    req[client_fd].outfile.flush();
    if (close)
        req[client_fd].outfile.close();
}

std::string generate_file_name(std::string &str, int client_fd, std::map<int, Request> &req)
{
    std::string time_tmp;
    std::stringstream s;
    s << Request::num_file++;
    time_tmp +=  s.str() ; 
    std::string pt;

    pt = time_tmp + get_file_extension(str, client_fd, req);              
    if (req[client_fd].target.find(".php") != req[client_fd].target.npos || req[client_fd].target.find(".py") != req[client_fd].target.npos)
        pt += ".txt";
    return pt;
}

void open_file(std::string &str, int client_fd, std::map<int, Request> &req)
{
    req[client_fd].outfile_name = req[client_fd].path_to_upload + generate_file_name(str, client_fd, req);
    req[client_fd].outfile.open(req[client_fd].outfile_name.c_str() , std::ios::binary);
    req[client_fd].open_boundry_file = 1;
}

void multiple_file_in_chunk(std::string &str, int client_fd, std::map<int, Request> &req, size_t pos)
{
    while(1)
    {
        if(req[client_fd].open_boundry_file == 0)
        {
            open_file(str,  client_fd, req);
        }

        else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos)
        {
            std::string s = str.substr(pos);
            if(strncmp(s.c_str(),(req[client_fd].boundary_separater + "--").c_str(), req[client_fd].boundary_separater.length() + 2 ) == 0)
            {
                write_content(client_fd, req, 1, str, pos);
                req[client_fd].epol = 0;
                return ;
            }
            else 
            {
                write_content(client_fd, req, 1, str, pos);
                req[client_fd].open_boundry_file = 0;
            }
        }
    }
}

void Boundry(int client_fd, std::map<int, Request> &req, std::string &str)
{
    size_t pos;
    size_t pos1;
   
    if (req[client_fd].open_boundry_file == 0)
        open_file(str,  client_fd, req);
    if ((str.find(req[client_fd].boundary_separater)) == str.npos && (str.find(req[client_fd].boundary_separater + "--")) == str.npos)
    { 
        int rest;
        if(str.length() > req[client_fd].boundary_separater.length())
           rest = str.length() - req[client_fd].boundary_separater.length();
        else
            rest = str.length();

        write_content(client_fd, req, 0, str, rest);
        req[client_fd].rest_of_boundry = str.substr(rest);
    }
    else if((pos = str.find(req[client_fd].boundary_separater)) != str.npos  && (pos1 = str.find(req[client_fd].boundary_separater + "--")) != str.npos && pos != pos1) 
    {
        write_content(client_fd, req, 1, str, pos);
        req[client_fd].open_boundry_file = 0;
        multiple_file_in_chunk(str, client_fd, req, pos);
    }
    else if((pos = str.find(req[client_fd].boundary_separater + "--")) != str.npos) //last separater
    {
        int rest = str.length() - (str.length() - pos);
        write_content(client_fd, req, 1, str, rest);
        
        req[client_fd].epol = 0;
        return ;
    }
    else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos  ) // midle separater
    {
        int rest = str.length() - (str.length() - pos);
        write_content(client_fd, req, 1, str, rest);

        req[client_fd].rest_of_boundry = str.substr(pos + 1);
        req[client_fd].open_boundry_file = 0; 
    }
}

int chunked_content_lenght(int client_fd, std::map<int, Request> &req)
{
    if (req[client_fd].maxbody > 0)
    {
        req[client_fd].max_readed += req[client_fd].chunk_size;
        if (req[client_fd].max_readed > req[client_fd].maxbody)
        {
            req[client_fd].outfile.close();
            req[client_fd].status = "413";
            req[client_fd].target = "error/413.html";
            std::remove(req[client_fd].outfile_name.c_str());
            req[client_fd].epol = 0;
            return 1;
        }
    }
    return 0;
}

void Chunked(int client_fd, std::map<int, Request> &req, std::string &str, int n_read)
{
    if (req[client_fd].calcul_chunk_flag == 0)
    {
        req[client_fd].chunk_size = get_chunk_size(str);
        if (req[client_fd].chunk_size == 0)
        {
            req[client_fd].outfile.close();
            req[client_fd].epol = 0;
            return ;
        }
        remove_size_of_chunk(str);
        req[client_fd].calcul_chunk_flag = 1;
    }
    if (req[client_fd].chunk_size  - ((int)str.size() - 2) < 0)
    {
        req[client_fd].outfile.write(str.c_str(), req[client_fd].chunk_size);
        req[client_fd].outfile.close();
        req[client_fd].epol = 0;
        return ;
    }
    req[client_fd].chunk_size -= str.size();
    if (n_read ==  req[client_fd].Bytes_readed - 1 && req[client_fd].Bytes_readed < 1024 )
        req[client_fd].outfile.write(str.c_str(), str.size() - 2);
    else
        req[client_fd].outfile.write(str.c_str(), str.size() );
    req[client_fd].outfile.flush();
    if (n_read < req[client_fd].Bytes_readed - 1)
        req[client_fd].Bytes_readed -= n_read;
    else if (req[client_fd].Bytes_readed < 1024)
    {
        req[client_fd].Bytes_readed = 1024;
        req[client_fd].calcul_chunk_flag = 0;
    }
    else if (req[client_fd].chunk_size < req[client_fd].Bytes_readed)
        req[client_fd].Bytes_readed = req[client_fd].chunk_size + 3;
}



void Chunked_helper(int client_fd, std::map<int, Request> &req, std::string &str)
{
    if (req[client_fd].calcul_chunk_flag == 0)
    {
        req[client_fd].chunk_size = get_chunk_size(str);
        
        if (req[client_fd].chunk_size == 0)
        {
            req[client_fd].outfile.close();
            req[client_fd].epol = 0;
            return ;
        }
        remove_size_of_chunk(str);
        req[client_fd].calcul_chunk_flag = 1;
    }
    if (req[client_fd].chunk_size  - ((int)str.size() - 2) <= 0)
    {
        req[client_fd].outfile.write(str.c_str(), req[client_fd].chunk_size);
        req[client_fd].outfile.close();
        req[client_fd].epol = 0;
        return ;
    }
    req[client_fd].chunk_size -= str.size();
    req[client_fd].outfile.write(str.c_str(), str.size());
   
}

void droping(std::vector<Server> &servers, epol *ep, int client_fd)
{
    epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    for (int j = 0; j < (int)servers.size(); j++)
    {
        for (int i = 0; i < (int)servers[j].fd_sock.size(); i++)
        {
            if (servers[j].fd_sock[i] == client_fd)
            {
                servers[j].fd_sock.erase(std::find(servers[i].fd_sock.begin(), servers[i].fd_sock.end(), client_fd));
                return ;
            }
        }
    }
        
}
void closing(int client_fd, std::map<int, Request> &req)
{
    req[client_fd].outfile.close();
    req[client_fd].fd_file.close();

}
void request_part(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req, std::map<int, std::string> &stringOfrequest)
{
    int  n_read;

    if (req.count(client_fd) == 0)
    {
        char rec[1024];

        memset(rec, 0, 1023);
        if ((n_read = read(client_fd, rec, 1023)) > 0)
            stringOfrequest[client_fd].append(rec,n_read);
        else if(n_read <= 0)
        {
            perror("read0 : ");
            droping(servers,ep,client_fd);
            return;
        }
        if (stringOfrequest[client_fd].find("\r\n\r\n") != std::string::npos || n_read < 1023)
        {
            int flag = 0;
            for (int j = 0; j < (int)servers.size(); j++)
            {
               for (int i = 0; i < (int)servers[j].fd_sock.size(); i++)
               {
                   if (servers[j].fd_sock[i] == client_fd)
                   {
                        Request obj(stringOfrequest[client_fd], servers[j], servers);
                        req.insert(std::pair<int, Request>(client_fd, obj));
                        flag = 1;
                        break;
                   }
                   if (flag == 1)
                       break;
               }
            }
            stringOfrequest.erase(client_fd);

            if(req[client_fd].status == "200" && req[client_fd].method == "POST")
            {
                if(req[client_fd].Post_status == "boundary")
                {   
                    if (req[client_fd].Body.find(req[client_fd].boundary_separater + "--") != std::string::npos)
                    {
                        req[client_fd].Body.clear();
                        Boundry(client_fd, req, req[client_fd].Body);
                    }
                }
               
                else if (req[client_fd].Post_status == "Bainary/Row")
                {
                    req[client_fd].lenght_Readed += req[client_fd].Body.size();
              

                    if (!req[client_fd].Body.empty())
                    {
                        req[client_fd].outfile.write(req[client_fd].Body.c_str(), req[client_fd].Body.size());
                        req[client_fd].Body.clear();
                    }
                    if (req[client_fd].lenght_Readed == req[client_fd].lenght_of_content)
                    {
                        req[client_fd].Body.clear();
                        req[client_fd].outfile.close();
                       
                        req[client_fd].epol = 0;
                        return;
                    }
                }
            
                else if(req[client_fd].Post_status == "chunked")
                {
                    if (!req[client_fd].Body.empty())
                    {
                        Chunked_helper(client_fd, req,req[client_fd].Body );
                        req[client_fd].Body.clear();
                    }
                }
            }
            if (req[client_fd].endOfrequest || req[client_fd].status != "200")
            {
                req[client_fd].epol = 0;
            }
        }
    }
    
    else
    {
        size_t size = req[client_fd].Bytes_readed;
        char rec_b[size];
        memset(rec_b, 0, size - 1);
        //
         if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
        {
            if (req[client_fd].Post_status == "Bainary/Row")
            {
                    req[client_fd].lenght_Readed += n_read;
                    req[client_fd].outfile.write(rec_b, n_read);
                    if (req[client_fd].lenght_Readed >= req[client_fd].lenght_of_content)
                    {
                        req[client_fd].outfile.close();
                        req[client_fd].epol = 0;
                        return;
                    }    
            }
            else if (req[client_fd].Post_status == "chunked")
            {
                std::string str;

                if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
                {
                    str.append(rec_b, n_read);
                    Chunked(client_fd, req, str, n_read);
                }
            }
            else if (req[client_fd].Post_status == "boundary")
            {
                if (!req[client_fd].Body.empty())
                {
                    std::string str;
                    str = req[client_fd].Body;
                    req[client_fd].Body.clear();
                    Boundry(client_fd, req, str);
                }
                else if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
                {
                    std::string str;
                    str.append(req[client_fd].rest_of_boundry);
                    str.append(rec_b, n_read);
                    Boundry(client_fd, req, str);
                }
            }
            memset(rec_b, 0, size - 1);
        }
        else if(n_read <= 0)
        {
            perror("read2 : ");
            droping(servers,ep,client_fd);
            closing( client_fd, req);
            std::map<int, Request>::iterator it = req.find(client_fd);
            if (it != req.end())
                req.erase(it);

        } 
    }
}

std::string get_last(std::string path)
{
    size_t h = 0;
    while ((h = path.find('/')) != std::string::npos)
    {

        if (h != path.length() - 1)
            path = path.substr(h + 1);
        else
            break;
    }
    return path;
}

struct cgi_args
{
    char *args[10];
    char *env[10];
};

void fill_envirements(cgi_args *cgi, std::map<int, Request> &req, int client_fd)
{
    std::string script;
    std::string path;

    script = "SCRIPT_FILENAME=" + req[client_fd].target ;
    path = "PATH_INFO=" + req[client_fd].target ;

  
    if ((req[client_fd].target.find(".php")) != std::string::npos)
        cgi->args[0] = (char*)"/usr/bin/php-cgi";
    else
        cgi->args[0] = (char*)"/usr/bin/python3.10" ;
    cgi->args[1] = (char*)req[client_fd].target.c_str();
    cgi->args[2] = NULL;
    if (req[client_fd].method == "GET")
    {
        cgi->env[0] =(char *)"REQUEST_METHOD=GET" ;
        cgi->env[1] = (char *)req[client_fd].query.c_str();
        cgi->env[2] = (char*)"REDIRECT_STATUS=200";
        cgi->env[3] = strdup(script.c_str());
        cgi->env[4] = strdup(path.c_str());
        cgi->env[5] = (char *)req[client_fd].cookie.c_str();
        cgi->env[6] = (char *)req[client_fd].accept.c_str();
        cgi->env[7] = NULL;
    }
    else if (req[client_fd].method == "POST")
    {
        cgi->env[0] =(char *)"REQUEST_METHOD=POST" ;
        cgi->env[1] = (char *)req[client_fd].content_lenght.c_str();
        cgi->env[2] = (char *)req[client_fd].content_type.c_str();
        cgi->env[3] = (char*)"REDIRECT_STATUS=200";
        cgi->env[4] = strdup(script.c_str());
        cgi->env[5] = strdup(path.c_str());
        cgi->env[6] = (char *)req[client_fd].accept.c_str();
        cgi->env[7] = (char *)req[client_fd].cookie.c_str();
        cgi->env[8] = (char *)"PHPRC=directorie/php.ini";

        cgi->env[9] = NULL;
    }
}

void forking( int client_fd, std::map<int, Request> &req)
{
    req[client_fd].is_forked_before = 1;
    if (pipe(req[client_fd].pipefd) == -1)
        perror("pipe");
    req[client_fd].pid_of_the_child = fork();

    if (req[client_fd].pid_of_the_child  == -1)
        perror("fork");
}

void child_proc(int client_fd, std::map<int, Request> &req)
{
    cgi_args args;
    close(req[client_fd].pipefd[0]); 
    dup2(req[client_fd].pipefd[1], 1); 
    close(req[client_fd].pipefd[1]);
    int fd ;
    if(req[client_fd].method == "POST")
    {
        fd = open(req[client_fd].outfile_name.c_str(),O_RDONLY | std::ios::binary);
        dup2(fd, 0);
        close (fd);
    }   
    fill_envirements(&args, req, client_fd);
    if (execve(args.args[0], args.args, args.env) == -1)
    {
        perror("exec = ");
    }
    exit(127);
}

int adding_pipe_2ep(epol *ep, int client_fd, std::map<int, Request> &req)
{
    ep->ev.events = EPOLLIN ;
    ep->ev.data.fd = req[client_fd].pipefd[0];
    if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, req[client_fd].pipefd[0], &ep->ev) == -1)
    {
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], &ep->ev);
        close(req[client_fd].pipefd[0]);
        return 0;
    }
    req[client_fd].time_of_child = clock();
    close(req[client_fd].pipefd[1]); 
    return 1;
}

int checking_timeout(int client_fd, std::map<int, Request> &req,Response& resp)
{
     pid_t retwait = waitpid(req[client_fd].pid_of_the_child, 0, WNOHANG);
    if (retwait == 0)
    {
        clock_t end = clock();
        double elapsed_seconds = static_cast<double>(end - req[client_fd].time_of_child) / CLOCKS_PER_SEC;
        if(elapsed_seconds >= 10)
        {
            std::remove(req[client_fd].outfile_name.c_str());
            close(req[client_fd].pipefd[0]); 
            kill(req[client_fd].pid_of_the_child, SIGKILL);
            req[client_fd].status = "504";
            resp.response_by_a_page("error/504.html");
            return 0;
        }
        else
            return 1;       
    }
    else
        req[client_fd].child_exited = 1;
    return 1;
}

int cgi_response(epol *ep, int client_fd, std::map<int, Request> &req,Response& resp)
{
    char buffer[70000];
    ssize_t bytesRead;
    if ((bytesRead = read(req[client_fd].pipefd[0], buffer, sizeof(buffer))) > 0)
    {        
        std::string header;
        std::string body;
        header.append(buffer,bytesRead);
        body.append(buffer,bytesRead);
        header = header.substr(0,header.find("\r\n\r\n")) ;
        body = body.substr(header.find("\r\n\r\n") + 4);
        std::string response_header = resp.cgi_header(header);
        response_header.append(buffer,bytesRead);
        if(write(client_fd,response_header.c_str(), response_header.size()) <= 0)
        {
            std::remove(req[client_fd].outfile_name.c_str());
            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], NULL);
            close(req[client_fd].pipefd[0]);
            return 0;
        }
        std::remove(req[client_fd].outfile_name.c_str());
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], NULL);
        close(req[client_fd].pipefd[0]);
    }
    else if(bytesRead <= 0)
        return 0;
    return 0;
}

int response(epol *ep, int client_fd, std::map<int, Request> &req,int fd_ready)
{

    signal(SIGPIPE, SIG_IGN);
    Response resp(client_fd,req);
    if (resp.is_delete() || resp.is_error()  )
    {
        
        resp.response_by_a_page(req[client_fd].target);
        

        return 0;
        
    }
    else if(resp.is_cgi() && req[client_fd].is_forked_before == 0 && req[client_fd].state_of_cgi == 1)
    { 
        
        if(req[client_fd].is_forked_before == 0)
            forking(client_fd,req);
        if (req[client_fd].pid_of_the_child== 0)   
            child_proc(client_fd,req);
        else 
            return (adding_pipe_2ep(ep,client_fd,req)); 
        return 1;
    }
    else if(resp.is_cgi() && req[client_fd].is_forked_before == 1 && req[client_fd].child_exited == 0)
    {
        return (checking_timeout(client_fd,req,resp));
    }
    else if(resp.is_cgi() && req[client_fd].is_forked_before == 1 && req[client_fd].child_exited == 1)
    {
        for(int i = 0 ;i < fd_ready ; i++ )
        {
            if(req[client_fd].pipefd[0] == ep->events[i].data.fd)
            {
                return(cgi_response(ep,client_fd,req,resp));
            }
        }
        return 1;
    }
    else if(resp.is_post())
    {
        std::string path;
        path = "succes.html";
       req[client_fd].status = "201";
        resp.response_by_a_page(path);
        return 0;
    }
    else if(resp.is_redirection())
    {
        std::string respon = resp.redirect_pages_header();
        if(write(client_fd, respon.c_str(), respon.size()) <= 0)
            perror("write6  ");
        return 0;
    }
    else if (resp.is_get())
    {
       
       if (resp.get_content_type() == "")
        {
            if (!resp.directorie_list())
                return 0;
        }
        else if (req[client_fd].header_flag == 0)
        {
            if (resp.chunked_response_headers())
                return 1;
        }
        else if (req[client_fd].header_flag == 1)
        {
            return (resp.chunked_response_body());
        }
    }
    return 0;
}

uint32_t ipToUint32(std::string &ip)
{
    std::vector<uint32_t> octets;
    std::istringstream iss(ip);
    std::string octet;

    while (std::getline(iss, octet, '.'))
    {
        octets.push_back(atoi(octet.c_str()));
    }

    return (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
}

void fill_ser_Add(Server &ser)
{
    ser.seraddr_s.sin_family = AF_INET;
    ser.seraddr_s.sin_addr.s_addr = htonl(ipToUint32(ser.host));
    ser.seraddr_s.sin_port = htons(ser.listen);
}

void init(Server &ser, epol *ep)
{
        if ((ser.server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            perror("failed to create socket");
        int reuse = 1;
        if (setsockopt(ser.server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            perror("setsockopt SO_REUSEADDR failed");
        fill_ser_Add(ser);
        if ((bind(ser.server_sock, (struct sockaddr *)&ser.seraddr_s, sizeof(ser.seraddr_s))) < 0)
            perror("bindddddd");
        if ((listen(ser.server_sock, 10)) < 0)
            perror("listen");

        ep->ev.events = EPOLLIN;
        ep->ev.data.fd = ser.server_sock;
        if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, ser.server_sock, &ep->ev) == -1)
        {
            close(ep->ep_fd);
            close(ser.server_sock);
            perror("epoll ctl");
        }
}

void accepting_new_clients(int i,Server& servers,epol *ep,std::map<int,clock_t> &timer)
{
   
        int client_fd = accept(ep->events[i].data.fd, NULL, NULL);

        if (client_fd == -1)
        {
            perror("accept fun");
            return;
        }
        timer[client_fd]= clock();
        ep->ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
        ep->ev.data.fd = client_fd;
        servers.fd_sock.push_back(client_fd);
        if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, client_fd, &ep->ev) == -1)
        {
            perror("epol_ctl");
            close(client_fd);
        }
}

void run(std::vector<Server> servers, epol *ep)
{
    std::map<int, Request> requests;
    std::map<int, clock_t> timer;
    std::map<int, std::string> stringOfrequest;
    while (1)
    {
        int fd_ready = epoll_wait(ep->ep_fd, ep->events, 1024, -1);
        if (fd_ready == -1)
            perror("epoll_wait");
        for (int i = 0; i < fd_ready; i++)
        {
            for (int j = 0; j < (int)servers.size(); j++)
            {
                if (servers[j].server_sock == ep->events[i].data.fd )
                {
                    accepting_new_clients(i,servers[j],ep,timer);
                }
            }
            for (int j = 0; j < (int)servers.size(); j++)
            {
               
                if (std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd) != servers[j].fd_sock.end())
                {
                    if(ep->events[i].events & EPOLLERR || ep->events[i].events & EPOLLHUP || ep->events[i].events & EPOLLRDHUP)
                    {
                        if(requests[ep->events[i].data.fd].is_forked_before == 1 && requests[ep->events[i].data.fd].child_exited == 0)
                        {
                            kill(requests[ep->events[i].data.fd].pid_of_the_child,SIGKILL);
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, requests[ep->events[i].data.fd].pipefd[0], NULL);
                            close(requests[ep->events[i].data.fd].pipefd[0]);
                        }
                        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                        close(ep->events[i].data.fd);
                        std::map<int, Request>::iterator it = requests.find(ep->events[i].data.fd);
                        if (it != requests.end())
                            requests.erase(it);
                        servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                    }
                    else if (ep->events[i].events & EPOLLIN && (requests.count(ep->events[i].data.fd) == 0 || requests[ep->events[i].data.fd].epol == 1)) ////////request
                        request_part(servers, ep, ep->events[i].data.fd, requests, stringOfrequest);
                    else if (ep->events[i].events & EPOLLOUT && requests.count(ep->events[i].data.fd) != 0 && requests[ep->events[i].data.fd].epol == 0 )
                    {
                       
                        if (!response(ep, ep->events[i].data.fd, requests,fd_ready))
                        {
                            closing( ep->events[i].data.fd, requests);
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                            close(ep->events[i].data.fd);
                            std::map<int, Request>::iterator it = requests.find(ep->events[i].data.fd);
                            if (it != requests.end())
                                requests.erase(it);
                            servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                        }
                    }
                    else if(requests.count(ep->events[i].data.fd) == 0)
                    {
                        
                        clock_t end = clock();
                        float elapsed_seconds = static_cast<float>(end - timer[ep->events[i].data.fd] ) / CLOCKS_PER_SEC;
                        if(elapsed_seconds >= 10)
                        {
                            
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                            close(ep->events[i].data.fd);
                            servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                            timer.erase(ep->events[i].data.fd);
                        }
                    }
                }

            }

        }
    }
}
