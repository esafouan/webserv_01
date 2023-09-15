#include "multi.hpp"
int Request::num_file = 0;
std::string construct_res_dir_list(const std::string &contentType, size_t contentLength)
{
    std::string header;

    // Status line
    header += "HTTP/1.1 200\r\n";

    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    // Blank line
    header += "\r\n";

    return header;
}

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
        return (".js");
    else if (content_type.find("application/json") != std::string::npos)
        return (".json");
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

void process_file_boundary(std::string filename, std::string separater)
{
    std::ifstream boundryfile(filename.c_str());
    std::ofstream myfile;
    std::string buffer;
    size_t pos;

    while (std::getline(boundryfile, buffer))
    {
        if ((pos = buffer.find("Content-Type")) != buffer.npos)
        {
            myfile.open(("directorie/upload/" + get_current_time() + get_extension(buffer.substr(pos + 1))).c_str());
            std::getline(boundryfile, buffer);
        }
        else if (buffer == (separater + "--" + "\r"))
        {
            myfile.close();
            break;
        }
        else if (buffer == separater + "\r")
        {
            myfile.close();
        }
        else
            myfile << buffer + "\n";
    }
    boundryfile.close();
    std::remove(filename.c_str());
}


size_t get_chunk_size(std::string str)
{
    size_t pos;
    pos = str.find("\r\n");
    // std::cout << "saad " << (int )str.substr(0, pos)[0] << std::endl;
    char *stat;
    // std::cout << "hex = " << str99substr(0, pos) << std::endl;
    // std::cout << "str = " << str << std::endl;
    size_t chunk_size = std::strtol(str.substr(0, pos).c_str(), &stat, 16);
    // std::cout << chunk_size << std::endl;
    return chunk_size;
}

void remove_size_of_chunk(std::string &str)
{
    str = str.substr(str.find("\n") + 1);
}
std::string get_file_extension(std::string &str)
{
    std::string ext;
    size_t pos;

    pos = str.find("Content-Type: ");
   
    str = str.substr(pos);

    pos = str.find("\n");
    ext = str.substr(14,pos - 15);
 
    pos = str.find("\r\n\r\n"); 
  
    str = str.substr(pos + 4);

   

    return (get_extension(ext));
}



void request_part(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req)
{
    int fd, n_read;
    off_t file_size;
    std::string request = "";
    char rec[3];

    memset(rec, 0, 2);

    if (req.count(client_fd) <= 0)
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
            if (req[client_fd].endOfrequest)
            {
                ep->ev.events = EPOLLOUT;
                ep->ev.data.fd = client_fd;
                epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
            }
        }
    }
    else
    {
        size_t size = req[client_fd].Bytes_readed;
        char rec_b[size];
        memset(rec_b, 0, size - 1);

        if (req[client_fd].Post_status == "Bainary/Row")
        {

            if ((n_read = read(client_fd, rec_b, 1999)) > 0)
            {

                req[client_fd].lenght_Readed += n_read;
                req[client_fd].outfile.write(rec_b, n_read);
                if (req[client_fd].lenght_Readed == req[client_fd].lenght_of_content)
                {
                    // std::cout << "closing " << ep->events[i].data.fd << std::endl;
                    req[client_fd].outfile.close();
                    ep->ev.events = EPOLLOUT;
                    ep->ev.data.fd = client_fd;
                    epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
                    return;
                }
                memset(rec_b, 0, 1999);
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
                        ep->ev.events = EPOLLOUT;
                        ep->ev.data.fd = client_fd;
                        epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
                        return ;
                    }
                    remove_size_of_chunk(str);
                    req[client_fd].calcul_chunk_flag = 1;
                }
                req[client_fd].chunk_size -= str.size();
                req[client_fd].outfile.write(str.c_str(), str.size());
                req[client_fd].outfile.flush();
                if (req[client_fd].chunk_size < req[client_fd].Bytes_readed)
                {
                    size = req[client_fd].chunk_size;
                    char tmp[size + 1];
                    str.clear();

                    if ((n_read = read(client_fd, tmp, size)) > 0)
                    {
                        str.append(tmp, n_read);
                        
                        if(n_read < size )
                        {
                            memset(tmp,0,n_read);
                            if ((n_read = read(client_fd, tmp, (size - n_read))) > 0)
                            {
        
                                str.append(tmp, n_read);
                            }
                                
                        }
                        req[client_fd].outfile.write(str.c_str(), str.size());
                        req[client_fd].outfile.flush();
                        memset(tmp, 0, size);
                    }
                    char separ[2];
                    read(client_fd, separ, 2);
                    req[client_fd].calcul_chunk_flag = 0;
                }       
                if(req[client_fd].calcul_chunk_flag == 1)
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
                    
                    std::string pt = time_tmp + get_file_extension(str);
            
                    req[client_fd].outfile.open(("directorie/upload/" + pt).c_str() , std::ios::binary);
                    req[client_fd].open_boundry_file = 1;

                }
                if ((str.find(req[client_fd].boundary_separater)) == str.npos && (str.find(req[client_fd].boundary_separater + "--")) == str.npos) //if no separater in the chunk
                {
                    int rest = str.length() - req[client_fd].boundary_separater.length();
                    req[client_fd].outfile.write(str.c_str(), rest);
                    req[client_fd].outfile.flush();
                    req[client_fd].rest_of_boundry = str.substr(rest);
                }
                 else if((pos = str.find(req[client_fd].boundary_separater)) != str.npos  && (str.find(req[client_fd].boundary_separater + "--")) != str.npos)
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
                            std::string pt = time_tmp + get_file_extension(str);
                            req[client_fd].outfile.open(("directorie/upload/" + pt).c_str() , std::ios::binary);
                            req[client_fd].open_boundry_file = 1;
                            
                        }
                        else if ((pos = str.find(req[client_fd].boundary_separater + "--")) != str.npos)
                        {
                            std::cout << str << std::endl;
                            req[client_fd].outfile.write(str.c_str(), pos);
                            req[client_fd].outfile.flush();
                            req[client_fd].outfile.close();

                            ep->ev.events = EPOLLOUT;
                            ep->ev.data.fd = client_fd;
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
                            return ;
                        }
                        else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos)
                        {
                            
                            req[client_fd].outfile.write(str.c_str(), pos);
                            req[client_fd].outfile.flush();
                            req[client_fd].outfile.close();
                            req[client_fd].open_boundry_file == 0;
                        }
                        
             
                        //std::cout <<"sdjjshd" <<std::endl;
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
                    ep->ev.events = EPOLLOUT;
                    ep->ev.data.fd = client_fd;
                    epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
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
    return path;
}

std::string generateDirectoryListing(const std::string &directoryPath,  std::map<int, Request> &req, int client_fd)
{
    std::stringstream htmlStream;
    htmlStream << "<html><body>\n";
    htmlStream << "<h1>Directory Listing: " << directoryPath << "</h1>\n";
    std::string haha = "";
    DIR *dir = opendir(directoryPath.c_str());

    if (dir)
    {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            std::string entryName = entry->d_name;
           
            if (entryName != "." && entryName != "..")
            {
                if(req[client_fd].flag_uri == 1)
                    htmlStream << "<p><a href=\"" << req[client_fd].uri_for_response + "/" << entryName << "\">" << entryName << "</a></p>\n";
                else 
                {

                    haha = get_last(directoryPath);
                    htmlStream << "<p><a href=\"" << haha << entryName << "\">" << entryName << "</a></p>\n";
                }
            }
        }
        closedir(dir);
    }
    else
    {
        htmlStream << "<p>Error opening directory.</p>\n";
    }

    htmlStream << "</body></html>\n";
    return htmlStream.str();
}

std::string construct_error_page(const std::string &contentType, size_t contentLength,std::string status)
{
   std::string header;

    
    // Status line
    header += "HTTP/1.1 ";
    header += status;
    header += "\r\n";
    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    std::stringstream contentLengthStream;
    contentLengthStream << contentLength;
    header += "Content-Length: " + contentLengthStream.str() + "\r\n";
    // Blank line
    header += "\r\n";

    return header;
}

std::string redirect_header(std::string target,std::string status)
{
   std::string header;

    
    // Status line
    header += "HTTP/1.1 ";
    header += status;
    header += "\r\n";
    header += "Location: ";
    header += target;
    // Content-Type header
    // header += "Content-Type: " + contentType + "\r\n";
    // std::stringstream contentLengthStream;
    // contentLengthStream << contentLength;
    // header += "Content-Length: " + contentLengthStream.str() + "\r\n";
    // Blank line
    header += "\r\n";

    return header;
}

void pages(std::string file_open,int client_fd,std::string status,std::string outfile_name)
{
    size_t file_size;
  

    int fd_file = open(file_open.c_str(), O_RDONLY);
    file_size = lseek(fd_file, 0, SEEK_END);
    lseek(fd_file, 0, SEEK_SET);
    std::string response_header = construct_error_page("text/html", file_size,status);
    write(client_fd, response_header.c_str(), response_header.size());
    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    ssize_t bytes_read;
    if ((bytes_read = read(fd_file, buffer, buffer_size)) > 0)
    {
        ssize_t bytes_sent = write(client_fd, buffer, bytes_read);
        if (bytes_sent == -1)
        {
            std::remove(outfile_name.c_str());
            err("Error sending data1");
        }
    }
    close(fd_file);
}

int response(epol *ep, int client_fd, std::map<int, Request> &req)
{
    signal(SIGPIPE, SIG_IGN);
    if ((req[client_fd].method == "DELETE"))
    {
        
        pages(req[client_fd].target,client_fd,req[client_fd].status,req[client_fd].outfile_name);
        return 0;
    }
    else if(req[client_fd].status == "301")
    {
        std::cout << "hna assi zby " << std::endl;
        std::string response_header = redirect_header(req[client_fd].target, req[client_fd].status);
        write(client_fd, response_header.c_str(), response_header.size());
        return 0;
    }
    else if(req[client_fd].status != "201" && req[client_fd].status != "200" && req[client_fd].status != "301")
    {
        pages(req[client_fd].target,client_fd,req[client_fd].status,req[client_fd].outfile_name);
        return 0;
  
    }
    else if (req[client_fd].method == "GET")
    {
        std::string target = req[client_fd].target;
        std::cout << "hna assi zby 1" << std::endl;
       if ((get_content_type(req[client_fd].target.c_str())) == "")
        {
            if (!directorie_list(target, client_fd, req))
                return 0;
        }
        else if (req[client_fd].header_flag == 0)
        {
            if (response_header(target, client_fd, req))
                return 1;
        }
        else if (req[client_fd].header_flag == 1)
        {
            return (chunked_response(target, client_fd, req));
        }
    }
    else if (req[client_fd].endOfrequest == 0 && (req[client_fd].method == "POST"))
    {
        req[client_fd].status = "201";
        pages("succes.html",client_fd,req[client_fd].status,req[client_fd].outfile_name);
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

void run(std::vector<Server> servers, epol *ep)
{
    // socklen_t len_cle = sizeof(client_addr);
    std::map<int, Request> requests;
    while (1)
    {
        // std::cout << "waiting for new client ..." << std::endl;
        int fd_ready = epoll_wait(ep->ep_fd, ep->events, 1, -1);
        // std::cout << "Event ..." << std::endl;
        if (fd_ready == -1)
            err("epoll_wait");
        for (int i = 0; i < fd_ready; ++i)
        {
            for (int j = 0; j < servers.size(); j++)
            {
                if (std::find(servers[j].server_sock.begin(), servers[j].server_sock.end(), ep->events[i].data.fd) != servers[j].server_sock.end())
                {
                    int client_fd = accept(ep->events[i].data.fd, NULL, NULL);
                    // std::cout << "catch a new connection " << client_fd << std::endl;
                    if (client_fd == -1)
                    {
                        std::cout << "accepting" << std::endl;
                        continue;
                    }
                    ep->ev.events = EPOLLIN;
                    ep->ev.data.fd = client_fd;
                    servers[j].fd_sock.push_back(client_fd);
                    if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, client_fd, &ep->ev) == -1)
                    {
                        std::cout << "epoll_ctl" << std::endl;
                        close(client_fd);
                    }
                }
            }
            for (int j = 0; j < servers.size(); j++)
            {
                if (std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd) != servers[j].fd_sock.end())
                {
                    if (ep->events[i].events & EPOLLIN) ////////request
                    {
                        request_part(servers, ep, ep->events[i].data.fd, requests);
                        continue;
                    }
                    else if (ep->events[i].events & EPOLLOUT)
                    {
                        if (!response(ep, ep->events[i].data.fd, requests))
                        {

                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                            std::cout << "closing " << ep->events[i].data.fd << std::endl;
                            close(ep->events[i].data.fd);
                            std::map<int, Request>::iterator it = requests.find(ep->events[i].data.fd);
                            if (it != requests.end())
                                requests.erase(it);
                            servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                        }
                    }
                }
            }

            // }

            // else
            // {
            //     if (events[i].events & EPOLLIN) ////////request
            //     {
            //         if (this->request_part(events[i].data.fd) == 1)
            //             continue;
            //     }
            //     else if (events[i].events & EPOLLOUT)
            //     {
            //         response(events[i].data.fd);
            //     }
            // }
        }
    }
}