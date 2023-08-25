#include "multi.hpp"

int soc::request_part(int fd_cl)
{
    int fd, n_read;
    off_t file_size;
    std::string request = "";
    char rec[LINE + 1];
    while ((n_read = read(fd_cl, rec, LINE + 1)) > 0)
    {

        request += rec;
        if (rec[n_read - 1] == '\n')
            break;
        memset(rec, 0, LINE);
    }
    if (n_read == 0)
    {
        close(fd_cl);
        epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd_cl, NULL);
        std::cout << "  client close the connction   " << std::endl;
        return 1;
    }

    else if (n_read < 0)
        err("read");
    // mahdi request
    if (n_read > 0)
    {
        Request obj(request, fd_cl);
        req.push_back(obj);
        ev.events = EPOLLOUT;
        ev.data.fd = fd_cl;
        epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd_cl, &ev);
    }
    return 0;
}
void soc::response(int fd_cl)
    {
        int reciver = -1;
        std::string target;
        int req_ind = -1;
        for (int j = 0; j < req.size(); j++)
        {
            if (req[j]._fd == fd_cl)
            {
                reciver = req[j]._fd;
                target = req[j].target.substr(1);
                req_ind = j;
            }
        }
        if (reciver != -1)
        {
            std::string dir;
            std::string content_type = birng_content(req, reciver);
            off_t file_size;
            std::string response_header;

            if (content_type == "")
            {
                dir = generateDirectoryListing("directorie");
                file_size = dir.size();
                response_header = constructResponseHeader("text/html", file_size, req);
                write(fd_cl, response_header.c_str(), response_header.size());
                const size_t buffer_size = 1024;
                char buffer[buffer_size];
                ssize_t bytes_read;
                ssize_t bytes_sent = write(fd_cl, dir.c_str(), dir.size());
                if (bytes_sent == -1)
                {
                    err("Error sending data");
                }
            }
            else
            {
                std::string file_open = "directorie/";
                file_open += target;
                int fd_file = open(file_open.c_str(), O_RDONLY);
                file_size = lseek(fd_file, 0, SEEK_END);
                lseek(fd_file, 0, SEEK_SET);
                response_header = constructResponseHeader(content_type, file_size, req);
                write(fd_cl, response_header.c_str(), response_header.size());
                const size_t buffer_size = 1024;
                char buffer[buffer_size];
                ssize_t bytes_read;
                while ((bytes_read = read(fd_file, buffer, buffer_size)) > 0)
                {
                    ssize_t bytes_sent = write(fd_cl, buffer, bytes_read);
                    if (bytes_sent == -1)
                    {
                        err("Error sending data1");
                    }
                }
                close(fd_file);
            }
            epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd_cl, NULL);
            close(fd_cl);
            req.erase(req.begin() + req_ind);
            // close
        }
    }
void soc::init()
{

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("failed to create socket");
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        err("setsockopt SO_REUSEADDR failed");
    this->fill_ser_Add(&seraddr);
    if ((bind(socket_fd, (struct sockaddr *)&seraddr, sizeof(seraddr))) < 0)
        err("bindddddd");
    if ((listen(socket_fd, 10)) < 0)
        err("listen");
    ep_fd = epoll_create(1);
    if (ep_fd == -1)
        err("epolllll");
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
    {
        close(ep_fd);
        close(socket_fd);
        err("epoll ctl");
    }
}

void soc::run()
{
    socklen_t len_cle = sizeof(client_addr);
    while (1)
    {
        int fd_ready = epoll_wait(ep_fd, events, 1, -1);
        if (fd_ready == -1)
            err("epoll_wait");
        for (int i = 0; i < fd_ready; ++i)
        {
            if (events[i].data.fd == socket_fd)
            {
                int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len_cle);
                if (client_fd == -1)
                {

                    std::cout << "accepting" << std::endl;
                    continue;
                }
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                {
                    std::cout << "epoll_ctl" << std::endl;
                    close(client_fd);
                }
            }
            else
            {
                if (events[i].events & EPOLLIN) ////////request
                {
                    if (this->request_part(events[i].data.fd) == 1)
                        continue;
                }
                else if (events[i].events & EPOLLOUT)
                {
                    response(events[i].data.fd);      
                }
            }
        }
    }
}