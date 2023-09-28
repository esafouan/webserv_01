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

    // Convert to a string and print
    char timeString[100]; // Adjust the buffer size as needed
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", std::localtime(&currentTime));
    std::string time(timeString);

    return (time);
}

std::string get_extension(std::string content_type)
{
    if (content_type.find("text/css") != std::string::npos)
        return (".css");
    else if (content_type.find("video/mp4") != std::string::npos)
        return (".mp4");
    else if (content_type.find("audio/mp4") != std::string::npos)
        return (".mp4");
    else if (content_type.find("text/csv") != std::string::npos)
        return (".csv");
    else if (content_type.find("image/gif") != std::string::npos)
        return (".gif");
    else if (content_type.find("text/html") != std::string::npos)
        return (".html");
    else if (content_type.find("image/x-icon") != std::string::npos)
        return (".ico");
    else if (content_type.find("image/jpeg") != std::string::npos)
        return (".jpeg");
    else if (content_type.find("image/jpeg") != std::string::npos)
        return (".jpg");
    else if (content_type.find("application/javascript") != std::string::npos)
        return (".py");
    else if (content_type.find("application/json") != std::string::npos)
        return (".pyon");
    else if (content_type.find("image/png") != std::string::npos)
        return (".png");
    else if (content_type.find("application/pdf") != std::string::npos)
        return (".pdf");
    else if (content_type.find("image/svg+xml") != std::string::npos)
        return (".svg");
    else if (content_type.find("text/plain") != std::string::npos)
        return (".txt");
    return ("");
}



size_t get_chunk_size(std::string str)
{
    size_t pos;
    pos = str.find("\r\n");
    char *stat;
    size_t chunk_size = std::strtol(str.substr(0, pos).c_str(), &stat, 16);
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
   
    pos = str.find("Content-Type: ");

    str = str.substr(pos);
    
    pos = str.find("\n");
    ext = str.substr(14,pos - 15);
    
    pos = str.find("\r\n\r\n"); 

    str = str.substr(pos + 4);
    req[client_fd].content_type_python = ext;
    return (get_extension(ext));
}



void request_part(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req)
{
    int fd, n_read;
    off_t file_size;
    std::string request = "";
    char rec[3];

    memset(rec, 0, 2);
    if (req.count(client_fd) == 0)
    {
        while ((n_read = read(client_fd, rec, 1)) > 0)
        {
            request += rec;
            if (request.find("\r\n\r\n") != std::string::npos)
                break;
            memset(rec, 0, 2);
        }
        if (n_read > 0)
        {
            int flag = 0;
            for (int j = 0; j < servers.size(); j++)
            {
                for (int i = 0; i < servers[j].fd_sock.size(); i++)
                {
                    if (servers[j].fd_sock[i] == client_fd)
                    {
                        Request obj(request, servers[j]);
                        req.insert(std::pair<int, Request>(client_fd, obj));

                        flag = 1;
                        break;
                    }
                    if (flag == 1)
                        break;
                }
            }
            
            if (req[client_fd].endOfrequest )
                req[client_fd].epol = 0;
        }
    }
    else
    {
        size_t size = req[client_fd].Bytes_readed;
        char rec_b[size];
        memset(rec_b, 0, size - 1);
        //
        if (req[client_fd].Post_status == "Bainary/Row")
        {

            if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
            {
               
                req[client_fd].lenght_Readed += n_read;
                req[client_fd].outfile.write(rec_b, n_read);
                if (req[client_fd].lenght_Readed == req[client_fd].lenght_of_content)
                {
                    // std::cout << "closing " << ep->events[i].data.fd << std::endl;
                    req[client_fd].outfile.close();
                   req[client_fd].epol = 0;
                    return;
                }
                memset(rec_b, 0, size - 1);
            }
        }
        
        else if (req[client_fd].Post_status == "chunked")
        {
            std::string str;
 
            if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
            {
                str.append(rec_b, n_read);
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
                if(n_read ==  req[client_fd].Bytes_readed - 1 && req[client_fd].Bytes_readed < 1024 )
                    req[client_fd].outfile.write(str.c_str(), str.size() - 2);
                else
                    req[client_fd].outfile.write(str.c_str(), str.size() );
                req[client_fd].outfile.flush();
                if(n_read < req[client_fd].Bytes_readed - 1)
                    req[client_fd].Bytes_readed -= n_read;
                else if(req[client_fd].Bytes_readed < 1024)
                {
                    req[client_fd].Bytes_readed = 1024;
                    req[client_fd].calcul_chunk_flag = 0;
                }
                else if (req[client_fd].chunk_size < req[client_fd].Bytes_readed)
                    req[client_fd].Bytes_readed = req[client_fd].chunk_size + 3;      
                memset(rec_b, 0, size - 1);
            }
        }
        
        else if (req[client_fd].Post_status == "boundary")
        {
            size_t pos;
            
            if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
            {
                std::string str;
                
                str.append(req[client_fd].rest_of_boundry);
                str.append(rec_b, n_read);
                if (req[client_fd].open_boundry_file == 0)
                {
                    std::string time_tmp;
                    std::stringstream s;
                    s << Request::num_file++;
                    time_tmp +=  s.str() ; 
                    std::string pt;

                    pt = time_tmp + get_file_extension(str, client_fd, req);              
                    if (req[client_fd].target.find(".php") != req[client_fd].target.npos || req[client_fd].target.find(".py") != req[client_fd].target.npos)
                    {
                        std::cout << "no"<<std::endl;
                        pt += ".txt";
                    }
                
                    req[client_fd].outfile_name = req[client_fd].path_to_upload + pt;
                    req[client_fd].outfile.open((req[client_fd].path_to_upload + pt).c_str() , std::ios::binary);
                    req[client_fd].open_boundry_file = 1;
                }
                if ((str.find(req[client_fd].boundary_separater)) == str.npos && (str.find(req[client_fd].boundary_separater + "--")) == str.npos) //if no separater in the chunk
                {
                    int rest = str.length() - req[client_fd].boundary_separater.length();
                    req[client_fd].outfile.write(str.c_str(), rest);
                    req[client_fd].outfile.flush();
                    req[client_fd].rest_of_boundry = str.substr(rest);  
                }
                else if((pos = str.find(req[client_fd].boundary_separater)) != str.npos  && (str.find(req[client_fd].boundary_separater + "--")) != str.npos) //both
                {
                    req[client_fd].outfile.write(str.c_str(), pos);
                    req[client_fd].outfile.flush();
                    req[client_fd].outfile.close();
                    req[client_fd].open_boundry_file = 0;
                    while(1)
                    {
                        //handle rest parts need cleaning code
                        if(req[client_fd].open_boundry_file == 0)
                        {
                            std::string time_tmp;
                            std::stringstream s;
                            s << Request::num_file++;
                            time_tmp +=  s.str() ;
                            std::string pt;
                            try
                            {
                                pt = time_tmp + get_file_extension(str, client_fd, req);        
                                if (req[client_fd].target.find(".php") != req[client_fd].target.npos || req[client_fd].target.find(".py") != req[client_fd].target.npos)
                                    pt += ".txt";
                                req[client_fd].outfile_name = req[client_fd].path_to_upload + pt;
                            }
                            catch(std::exception &e)
                            {
                                req[client_fd].epol = 0;
                                return ;  
                            }
                            req[client_fd].outfile.open((req[client_fd].path_to_upload + pt).c_str() , std::ios::binary);
                            req[client_fd].open_boundry_file = 1;
                            
                        }
                        else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos)
                        {
                            std::string s = str.substr(pos);
                            if(strncmp(s.c_str(),(req[client_fd].boundary_separater + "--").c_str(),req[client_fd].boundary_separater.length() + 2 ) == 0)
                            {
                                
                                req[client_fd].outfile.write(str.c_str(), pos);
                                req[client_fd].outfile.flush();
                                req[client_fd].outfile.close();
                                req[client_fd].epol = 0;
                                return ;
                            }
                            else 
                            {
                                req[client_fd].outfile.write(str.c_str(), pos);
                                req[client_fd].outfile.flush();
                                req[client_fd].outfile.close();
                                req[client_fd].open_boundry_file = 0;
                            }
                        }
                    }
                }
                
                else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos  ) // midle separater
                {
                    int rest = str.length() - (str.length() - pos);

                    req[client_fd].outfile.write(str.c_str(), rest);
                    req[client_fd].outfile.flush();
                    req[client_fd].rest_of_boundry = str.substr(pos + 1);
                    req[client_fd].open_boundry_file = 0;
                    req[client_fd].outfile.close();
                }
               
                else if((pos = str.find(req[client_fd].boundary_separater + "--")) != str.npos) //last separater
                {
                    int rest = str.length() - (str.length() - pos);

                    req[client_fd].outfile.write(str.c_str(), rest);
                    req[client_fd].outfile.flush();
                    req[client_fd].outfile.close();
                    req[client_fd].epol = 0;
                    return ;
                }
                memset(rec_b, 0, size - 1);
            }    
        }
    }
    if (n_read == 0)
    {
        close(client_fd);
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
        std::cout << "  client close the connction   " << std::endl;
        return;
    }
    else if (n_read < 0)
        perror("read ");
    return;
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
    // std::cout <<"here " <<path << std::endl;
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
    path = "PATH_INFO=" + req[client_fd].target ;//req[client_fd].target

    //std::cerr << "URI -> " << req[client_fd].target << std::endl;
  
    if ((req[client_fd].target.find(".php")) != std::string::npos)
        cgi->args[0] = (char*)"/usr/bin/php-cgi";
    else
        cgi->args[0] = (char*)"/usr/bin/python3.10" ;
    cgi->args[1] = (char*)req[client_fd].target.c_str();
    cgi->args[2] = NULL;
    //env
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
        cgi->env[8] = NULL;
    }
}
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

int response(epol *ep, int client_fd, std::map<int, Request> &req,int fd_ready)
{
    signal(SIGPIPE, SIG_IGN);
    Response resp(client_fd,req);
    if(resp.is_cgi() && req[client_fd].is_forked_before == 0)
    {
        
        if(req[client_fd].is_forked_before == 0)
        {
            req[client_fd].is_forked_before = 1;
            // if (pipe(req[client_fd].pipefd) == -1)
            // {
            //     perror("pipe");
            // }
            req[client_fd].fd_f = open("directorie/test.txt", O_CREAT | O_RDWR , 0777);
            // exit(1);
            // std::cerr <<"pp = " <<req[client_fd].fd_f << std::endl;
            // exit(1);
            req[client_fd].pid_of_the_child = fork();

            if (req[client_fd].pid_of_the_child  == -1)
            {
                perror("fork");
            }
        }
        if (req[client_fd].pid_of_the_child== 0) 
        {   
            cgi_args args;
            // close(req[client_fd].pipefd[0]); 
            // dup2(req[client_fd].pipefd[1], 1); 
            dup2(req[client_fd].fd_f, 1); 

            // close(req[client_fd].pipefd[1]);
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
        else 
        { 
           
            // ep->ev.events = EPOLLIN ;
            // ep->ev.data.fd = req[client_fd].pipefd[0];
            // if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, req[client_fd].pipefd[0], &ep->ev) == -1)
            // {
            //     exit(1);
            //     close(req[client_fd].pipefd[0]);
            // }
            close(req[client_fd].fd_f);
            req[client_fd].time_of_child = clock();
            // close(req[client_fd].pipefd[1]); 
        }
        return 1;
    }
    else if(resp.is_cgi() && req[client_fd].is_forked_before == 1 && req[client_fd].child_exited == 0)
    {
        pid_t retwait = waitpid(req[client_fd].pid_of_the_child, 0, WNOHANG);
        if (retwait == 0)
        {
            clock_t end = clock();
            double elapsed_seconds = static_cast<double>(end - req[client_fd].time_of_child) / CLOCKS_PER_SEC;
            if(elapsed_seconds >= 50)
            {
                kill(req[client_fd].pid_of_the_child, SIGKILL);
                req[client_fd].status = "503";
                resp.response_by_a_page("error/503.html");
            }
            else
            {  
                return 1;       
            }
        }
        else
        {
            req[client_fd].child_exited = 1;
        }
        return 1;
    }
    else if(resp.is_cgi() && req[client_fd].is_forked_before == 1 && req[client_fd].child_exited == 1)
    {

        // for(int i = 0 ;i < fd_ready ; i++ )
        // {
        //     if(req[client_fd].pipefd[0] == ep->events[i].data.fd)
        //     {
                char buffer[1024];
                ssize_t bytesRead;
                int fd = open("directorie/test.txt" , O_RDONLY , 0777);
                if ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
                {
                    std::cerr << bytesRead << std::endl;
                    if(req[client_fd].target.find("data.py") != std::string::npos)
                    {
                        std::string tmp(buffer);

                        size_t pos;
                        if ((pos = tmp.find("Set-Cookie")) != tmp.npos)
                        {
                            tmp = tmp.substr(pos);
                            pos = tmp.find("\n");
                            tmp = tmp.substr(0, pos);
                        }
                        std::string response_header = resp.normal_pages_header1(bytesRead , tmp);
                        write(client_fd, response_header.c_str(), response_header.size());
                        write(client_fd,buffer,bytesRead);
                    }
                    else
                    {
                        std::string header;
                        std::string body;
                        header.append(buffer,bytesRead);
                        body.append(buffer,bytesRead);
                        header = header.substr(0,header.find("\r\n\r\n")) ;
                        body = body.substr(header.find("\r\n\r\n") + 4);
                        std::string response_header = resp.normal_pages_header1(bytesRead , header);
                        write(client_fd, response_header.c_str(), response_header.size());
                        write(client_fd,body.c_str(),body.size());
                    }
                    
                    
                    // epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], NULL);
                    // close(req[client_fd].pipefd[0]);
                    
                    return 1;
                }
                else if(bytesRead == 0)
                {
                    std::remove(req[client_fd].outfile_name.c_str());
                    return 0;            // }
                }
           
        // }
        return 1;
        
    }
    else if (resp.is_delete() || resp.is_error() || resp.is_post())
    {
        std::string path;
        if(resp.is_delete() || resp.is_error())
            path = req[client_fd].target;
        else
        {
            path = "succes.html";
             req[client_fd].status = "201";
        }
        resp.response_by_a_page(path);
        return 0;
    }
    else if(resp.is_redirection())
    {
        write(client_fd, resp.redirect_pages_header().c_str(), resp.redirect_pages_header().size());
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

void fill_ser_Add(Server &ser, int i)
{
    ser.seraddr_s[i].sin_family = AF_INET;
    // ser.seraddr_s[i].sin_addr.s_addr = htonl(ipToUint32(ser.host));
    ser.seraddr_s[i].sin_addr.s_addr = INADDR_ANY;
    ser.seraddr_s[i].sin_port = htons(ser.listen[i]);
}

void init(Server &ser, epol *ep)
{
    for (int i = 0; i < ser.listen.size(); i++)
    {
        if ((ser.server_sock[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            err("failed to create socket");
        int reuse = 1;
        if (setsockopt(ser.server_sock[i], SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            err("setsockopt SO_REUSEADDR failed");
        fill_ser_Add(ser, i);
        if ((bind(ser.server_sock[i], (struct sockaddr *)&ser.seraddr_s[i], sizeof(ser.seraddr_s[i]))) < 0)
            err("bindddddd");
        if ((listen(ser.server_sock[i], 10)) < 0)
            err("listen");

        ep->ev.events = EPOLLIN;
        ep->ev.data.fd = ser.server_sock[i];
        if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, ser.server_sock[i], &ep->ev) == -1)
        {
            close(ep->ep_fd);
            close(ser.server_sock[i]);
            err("epoll ctl");
        }
    }
}

void accepting_new_clients(int i,int j,std::vector<Server>& servers,epol *ep,std::map<int, Request>& requests)
{
    if (std::find(servers[j].server_sock.begin(), servers[j].server_sock.end(), ep->events[i].data.fd) != servers[j].server_sock.end())
    {
        int client_fd = accept(ep->events[i].data.fd, NULL, NULL);
        if (client_fd == -1)
        {
            std::cout << "accepting" << std::endl;
            return;
        }
        ep->ev.events = EPOLLIN | EPOLLOUT;
        ep->ev.data.fd = client_fd;
        servers[j].fd_sock.push_back(client_fd);
        if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, client_fd, &ep->ev) == -1)
        {
            std::cout << "epoll_ctl" << std::endl;
            close(client_fd);
        }
    }
}

void run(std::vector<Server> servers, epol *ep)
{
    // socklen_t len_cle = sizeof(client_addr);
    std::map<int, Request> requests;
    while (1)
    {
        // std::cout << "waiting for new client ..." << std::endl;
        int fd_ready = epoll_wait(ep->ep_fd, ep->events, 10, -1);
        
        if (fd_ready == -1)
            err("epoll_wait");
        for (int i = 0; i < fd_ready; i++)
        {
            for (int j = 0; j < servers.size(); j++)
            {
                accepting_new_clients(i,j,servers,ep,requests);
            }
            for (int j = 0; j < servers.size(); j++)
            {
                if (std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd) != servers[j].fd_sock.end())
                {
                    if (ep->events[i].events & EPOLLIN && (requests.count(ep->events[i].data.fd) == 0 || requests[ep->events[i].data.fd].epol == 1)) ////////request
                    {
                        request_part(servers, ep, ep->events[i].data.fd, requests);
                        continue;
                    }
                    else if (ep->events[i].events & EPOLLOUT && requests.count(ep->events[i].data.fd) != 0 && requests[ep->events[i].data.fd].epol == 0 )
                    {
                        if (!response(ep, ep->events[i].data.fd, requests,fd_ready))
                        {
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                            close(ep->events[i].data.fd);
                            std::map<int, Request>::iterator it = requests.find(ep->events[i].data.fd);
                            if (it != requests.end())
                                requests.erase(it);
                            servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                        }
                    }
                }

            }

        }
    }
}



