#include "multi.hpp"

void request_part(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req)
{
    int fd, n_read;
    off_t file_size;
    std::string request = "";
    char rec[LINE + 1];
    while ((n_read = recv(client_fd, rec, LINE + 1, 0)) > 0)
    {
        request += rec;
        if (rec[n_read - 1] == '\n')
            break;
        memset(rec, 0, LINE);
    }
    if (n_read == 0)
    {
        close(client_fd);
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
        std::cout << "  client close the connction   " << std::endl;
        return ;
    }
    else if (n_read < 0)
    {
        perror("read ");
        // std::cout << client_fd << " failed" << std::endl;
        //  err("read");
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
        ep->ev.events = EPOLLOUT;
        ep->ev.data.fd = client_fd;
        epoll_ctl(ep->ep_fd, EPOLL_CTL_MOD, client_fd, &ep->ev);
    }
    return ;
}
int response(epol *ep, int client_fd, std::map<int, Request> &req)
{
    std::string target = req[client_fd].target;
    std::string target_to_send = get_content_type(target.c_str());
    int fd_file = open(target.c_str(), O_RDONLY);

    std::string response_header = constructResponseHeader(target_to_send, req[client_fd].status);
    send(client_fd, response_header.c_str(), response_header.size(),0);

    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    ssize_t bytes_read;
    while ((bytes_read = read(fd_file, buffer, buffer_size)) > 0)
    {
        // chunks
        std::stringstream chunk_size_stream;
        chunk_size_stream << std::hex << bytes_read << "\r\n";
        std::string chunk_size_line = chunk_size_stream.str();
        send(client_fd, chunk_size_line.c_str(), chunk_size_line.size(),0);
        // chunks
        ssize_t bytes_sent = send(client_fd, buffer, bytes_read,0);
        if (bytes_sent == -1)
        {
            perror("Errors ");
            close(fd_file);
            return 0;
        }
        write(client_fd, "\r\n", 2);
    }
    if (bytes_read == 0)
    {
        write(client_fd, "0\r\n\r\n", 5);
        close(fd_file);
        return 0;
    }
    else if (bytes_read > 0)
    {
        return 1;
    }
    return 1;
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
        int fd_ready = epoll_wait(ep->ep_fd, ep->events, 1, -1);
        if (fd_ready == -1)
            err("epoll_wait");
        for (int i = 0; i < fd_ready; ++i)
        { 
            for (int j = 0; j < servers.size(); j++)
            {
                if (std::find(servers[j].server_sock.begin(), servers[j].server_sock.end(), ep->events[i].data.fd) != servers[j].server_sock.end())
                {
                    int client_fd = accept(ep->events[i].data.fd, NULL, NULL);
                    if (client_fd == -1)
                    {

                        std::cout << "accepting" << std::endl;
                        continue;
                    }
                    ep->ev.events = EPOLLIN;
                    ep->ev.data.fd = client_fd;
                    servers[j].fd_sock.push_back(client_fd);
                    std::cout <<"acepting" <<"client = " <<client_fd << " from serve "<< j << std::endl;
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
                    std::cout <<"request from " <<"client = " <<ep->events[i].data.fd << " from serve "<< j << std::endl;
                    if (ep->events[i].events & EPOLLIN) ////////request
                    {
                        request_part(servers, ep, ep->events[i].data.fd, requests);
                        continue;
                    }
                    else if (ep->events[i].events & EPOLLOUT)
                    {
                        if(!response(ep, ep->events[i].data.fd, requests))
                        {
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                            close(ep->events[i].data.fd);
                            std::map<int , Request >::iterator it = requests.find(ep->events[i].data.fd);
                            if(it != requests.end())
                                requests.erase(it);
                            servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(),servers[j].fd_sock.end(),ep->events[i].data.fd));
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