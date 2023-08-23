#include "multi.hpp"
#define LINE 4096
void fill_ser_Add(sockaddr_in *add,std::vector<Server> &ser)
{
    add->sin_family = AF_INET;
    add->sin_addr.s_addr = htonl(INADDR_ANY);
    add->sin_port = htons(ser[0].listen[0]);
}
void err(std::string str)
{
    std::cout << str << std::endl;
    _exit(1);
}
int main(int ac,char **av)
{
    if(ac != 2)
    {
        std::cout << "configue file needed" << std::endl;
        return 0; 
    }
    int serv_fd;
    std::vector<Request> requests;

    struct sockaddr_in seraddr, client_addr;
    int fd_ready;
    std::vector<Server> servers = mainf(ac,av);
    if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("failed to create socket");
    int reuse = 1;
    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        err("setsockopt SO_REUSEADDR failed");
    fill_ser_Add(&seraddr,servers);
    if ((bind(serv_fd, (struct sockaddr *)&seraddr, sizeof(seraddr))) < 0)
        err("bindddddd");
    if ((listen(serv_fd, 10)) < 0)
        err("listen");
    int fd_epo = epoll_create(1);
    if (fd_epo == -1)
        err("epolllll");
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = serv_fd;
    if (epoll_ctl(fd_epo, EPOLL_CTL_ADD, serv_fd, &ev) == -1)
    {
        close(fd_epo);
        close(serv_fd);
        err("epoll ctl");
    }
    struct epoll_event events[10];
    socklen_t len_cle = sizeof(client_addr);
     std::vector<int> client_fds;
    while (1)
    {
        fd_ready = epoll_wait(fd_epo, events, 10, -1);
        if (fd_ready == -1)
            err("epoll_wait");
        for (int i = 0; i < fd_ready; ++i)
        {
            if (events[i].data.fd == serv_fd)
            {
                int client_fd = accept(serv_fd, (struct sockaddr *)&client_addr, &len_cle);
                if (client_fd == -1)
                {

                    std::cout << "accepting" << std::endl;
                    continue;
                }
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(fd_epo, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                {
                    std::cout << "epoll_ctl" << std::endl;
                    close(client_fd);
                }
                else
                    client_fds.push_back(client_fd);
            }
            else
            {
                ////////request
                int fd,n_read;
                off_t file_size;
                std::string request = "";

                char rec[LINE + 1];
                fd = open("home.html", O_RDONLY);
                file_size = lseek(fd, 0, SEEK_END);
                lseek(fd, 0, SEEK_SET);
                while ((n_read = read(events[i].data.fd, rec, 3999)) > 0)
                {
                    //std::cout << "1 -> "<< rec << " <- 2"<< std::endl;
                    request += rec;
                    if (rec[n_read - 1] == '\n')
                        break;
                    memset(rec, 0, LINE);
                }

                if (n_read == 0)
                {
                    epoll_ctl(fd_epo, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    std::cout << "client close the connction   " << std::endl;
                }

                else if (n_read < 0)
                    err("read");

                //mahdi request

                if (n_read != 0)
                {
                    Request obj(request);
                    requests.push_back(obj);
                }

            }   
        }
    }
}