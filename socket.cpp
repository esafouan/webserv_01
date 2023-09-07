#include "multi.hpp"

std::string construct_res_dir_list(const std::string &contentType, size_t contentLength)
{
    std::string header;

    // Status line
    header += "HTTP/1.1 200 OK\r\n";

    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    // Blank line
    header += "\r\n";

    return header;
}

int saad(std::string s)
{
    std::ifstream file(s.c_str(), std::ios::binary);

    if (!file)
    {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    // Move the file pointer back by 10 characters (assuming there are at least 10 characters in the file)
    file.seekg(-5, std::ios::end);

    // Read the last 10 characters
    char buffer[6]; // 10 characters + null-terminator
    file.read(buffer, 5);

    // Null-terminate the buffer
    buffer[5] = '\0';

    // Print the last 10 characters
    std::string str(buffer);

    if (str.find("0\r\n\r\n") != str.npos)
    {
        file.close();
        return 0;
    }
    file.close();

    return 1;
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

    //std::cout << "sok sep = "<< separater << std::endl;
    while (std::getline(boundryfile, buffer))
    {
        if ((pos = buffer.find("Content-Type")) != buffer.npos)
            myfile.open(("directorie/upload/" + get_current_time() + get_extension(buffer.substr(pos + 1))).c_str());  
        else if (buffer == (separater + "--" + "\r"))
        {
            
            myfile.close();
            break;
        }
        else if (buffer == separater + "\r")
        {
            myfile.close();
        }
        else if (buffer != "\r\n" && buffer != "\r")
        {
            myfile << buffer + "\n";
        }
         
    }
    boundryfile.close();
    std::remove(filename.c_str());

}

void process_file(std::string s, std::string extension, std::string separater, int type)
{
    std::ifstream file(s.c_str(), std::ios::binary);
    std::string name;
    
    if(type)
        name = "directorie/upload/" + get_current_time() + extension;
    else
        name = "directorie/" + get_current_time() + ".txt";

    std::ofstream uploaded_file(name.c_str(), std::ios::binary);

    while (1)
    {
        char *stat;
        std::string size;
        size_t n_read;
        std::getline(file, size);

        if (size.empty() || size == "0\r\n")
            break;
        size_t chunk_size = std::strtol(size.c_str(), &stat, 16);
        if (!chunk_size)
            break;
        char buff[chunk_size + 1];
        char separator[2];
        file.read(buff, chunk_size);
        file.read(separator, 2);
        uploaded_file.write(buff, chunk_size);
    }
    file.close();
    uploaded_file.close();
    std::remove(s.c_str());
    if(!type)
        process_file_boundary(name, separater);
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
            {
                break;
            }
            memset(rec, 0, 2);
        }

        if (n_read == 0)
        {
            close(client_fd);
            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
            std::cout << "  client close the connction   " << std::endl;
            return;
        }
        else if (n_read < 0)
        {
            perror("read ");
        }
        // mahdi request
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
        char rec_b[2000];

        memset(rec_b, 0, 1999);
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

            if (n_read == 0)
            {
                close(client_fd);
                epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
                std::cout << "  client close the connction   " << std::endl;
                return;
            }
            else if (n_read < 0)
            {
                perror("read ");
                // std::cout << client_fd << " failed" << std::endl;
                //  err("read");
            }
        }
        
        else if (req[client_fd].Post_status == "chunked") // msdmfsdmnsdmkcj
        {
            // std::ofstream saad1("ss.txt", std::ios::app | std::ios::binary);
            if ((n_read = read(client_fd, rec_b, 1999)) > 0)
            {
                req[client_fd].outfile.write(rec_b, n_read);
                req[client_fd].outfile.flush();
                std::string str;
                str.append(rec_b,n_read);

                if (str.find("0\r\n\r\n") != str.npos)
                {
                    process_file(req[client_fd].outfile_name, req[client_fd].extension, "", 1);
                    req[client_fd].outfile.close();
                    ep->ev.events = EPOLLOUT;
                    ep->ev.data.fd = client_fd;
                    epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
                    return;
                }
                memset(rec_b, 0, 1999);
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
        }

        else if (req[client_fd].Post_status == "boundary")
        {
            // std::ofstream saad1("ss.txt", std::ios::app | std::ios::binary);
            if ((n_read = read(client_fd, rec_b, 1999)) > 0)
            {
                req[client_fd].outfile.write(rec_b, n_read);
                req[client_fd].outfile.flush();
                std::string str;
                str.append(rec_b, n_read);
                if (str.find(req[client_fd].boundary_separater + "--") != str.npos)
                {
                    
                    process_file_boundary(req[client_fd].outfile_name, req[client_fd].boundary_separater);

                    req[client_fd].outfile.close();
                    ep->ev.events = EPOLLOUT;
                    ep->ev.data.fd = client_fd;
                    epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
                    return;
                }
                memset(rec_b, 0, 1999);
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
        }

        else if (req[client_fd].Post_status == "Chunked/boundary")
        {
           if ((n_read = read(client_fd, rec_b, 1999)) > 0)
            {
                req[client_fd].outfile.write(rec_b, n_read);
                req[client_fd].outfile.flush();
                std::string str;
                str.append(rec_b, n_read);
                if (str.find("0\r\n\r\n") != str.npos)
                {
                        process_file(req[client_fd].outfile_name, "", req[client_fd].boundary_separater, 0);
                        std::cout << str << std::endl;
                        req[client_fd].outfile.close();
                        ep->ev.events = EPOLLOUT;
                        ep->ev.data.fd = client_fd;
                        epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
                        return ;
                }
                memset(rec_b, 0, 1999);
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
        }
    }
    return;
}

std::string get_last(std::string path)
{
    size_t h = 0;

    while (path.find('/') != std::string::npos)
    {
        h = path.find('/');
        path = path.substr(h + 1);
    }
    return path;
}

std::string generateDirectoryListing(const std::string &directoryPath)
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
            haha = get_last(directoryPath) + "/";

            if (entryName != "." && entryName != "..")
            {
                // std::cout << "url =" << directoryPath << "/" << entryName << std::endl;
                htmlStream << "<p><a href=\"" << haha << entryName << "\">" << entryName << "</a></p>\n";
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

// std::string generateDirectoryListing(const std::string &directoryPath)
// {
//     std::stringstream htmlStream;
//     htmlStream << "<html><body>\n";
//     htmlStream << "<h1>Directory Listing: " << directoryPath << "</h1>\n";

//     DIR *dir = opendir(directoryPath.c_str());
//     if (dir)
//     {
//         struct dirent *entry;
//         while ((entry = readdir(dir)) != NULL)
//         {
//             std::string entryName = entry->d_name;
//             if (entryName != "." && entryName != "..")
//             {
//                 htmlStream << "<p><a href=\"" << entryName << "\">" << entryName << "</a></p>\n";
//             }
//         }
//         closedir(dir);
//     }
//     else
//     {
//         htmlStream << "<p>Error opening directory.</p>\n";
//     }

//     htmlStream << "</body></html>\n";
//     return htmlStream.str();
// }
std::string mama(const std::string &contentType, size_t contentLength)
{
    std::string header;

    // Status line
    header += "HTTP/1.1 302 OK\r\n";

    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    //    header += "Location: /directorie/succes.html\r\n";
    // Content-Length header
    std::stringstream contentLengthStream;
    contentLengthStream << contentLength;
    header += "Content-Length: " + contentLengthStream.str() + "\r\n";

    // Blank line
    header += "\r\n";

    return header;
}

int response(epol *ep, int client_fd, std::map<int, Request> &req)
{
    signal(SIGPIPE, SIG_IGN);
    if (req[client_fd].method == "GET")
    {
        std::string target = req[client_fd].target;
        if ((get_content_type(req[client_fd].target.c_str())) == "")
        {
            if (!directorie_list(target, client_fd))
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
    else if (req[client_fd].endOfrequest == 0 && req[client_fd].method == "POST")
    {
        size_t file_size;
        std::string file_open = "directorie/succes.html";
        int fd_file = open(file_open.c_str(), O_RDONLY);
        file_size = lseek(fd_file, 0, SEEK_END);
        lseek(fd_file, 0, SEEK_SET);
        std::string response_header = mama("text/html", file_size);
        write(client_fd, response_header.c_str(), response_header.size());
        const size_t buffer_size = 1024;
        char buffer[buffer_size];
        ssize_t bytes_read;
        if ((bytes_read = read(fd_file, buffer, buffer_size)) > 0)
        {
            ssize_t bytes_sent = write(client_fd, buffer, bytes_read);
            if (bytes_sent == -1)
                err("Error sending data1");
        }
        close(fd_file);
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
    ser.seraddr_s[i].sin_addr.s_addr = htonl(ipToUint32(ser.host));
    // ser.seraddr_s[i].sin_addr.s_addr = INADDR_ANY;
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
                            // std::cout << "closing " << ep->events[i].data.fd << std::endl;
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