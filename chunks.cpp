
#include "multi.hpp"
#define LINE 4096
void fill_ser_Add(sockaddr_in *add, std::vector<Server> &ser)
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
std::string constructResponseHeader1(const std::string &contentType, size_t contentLength, std::vector<Request> req)
{
    std::string header;

    (void)req;
    // Status line
    header += "HTTP/1.1 200 OK\r\n";

    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    // Content-Length header
    std::stringstream contentLengthStream;
    contentLengthStream << contentLength;
    header += "Content-Length: " + contentLengthStream.str() + "\r\n";

    // Blank line
    header += "\r\n";

    return header;
}
std::string constructResponseHeader(const std::string &contentType, size_t contentLength, std::vector<Request> req)
{
    std::string header;

    (void)req;
    // Status line
    header += "HTTP/1.1 200 OK\r\n";

    // Content-Type header
    header += "Content-Type: " + contentType + "\r\n";
    header += "Transfer-Encoding: chunked\r\n";
    // Content-Length header
    std::stringstream contentLengthStream;
    contentLengthStream << contentLength;
    header += "Content-Length: " + contentLengthStream.str() + "\r\n";

    // Blank line
    header += "\r\n";

    return header;
}
std::string get_content_type(const char *path)
{
    const char *last_dot = strrchr(path, '.');
    if (last_dot)
    {
        if (strcmp(last_dot, ".css") == 0)
            return "text/css";
        if (strcmp(last_dot, ".mp4") == 0)
            return "video/mp4";
        if (strcmp(last_dot, ".csv") == 0)
            return "text/csv";
        if (strcmp(last_dot, ".gif") == 0)
            return "image/gif";
        if (strcmp(last_dot, ".htm") == 0)
            return "text/html";
        if (strcmp(last_dot, ".html") == 0)
            return "text/html";
        if (strcmp(last_dot, ".ico") == 0)
            return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0)
            return "application/javascript";
        if (strcmp(last_dot, ".json") == 0)
            return "application/json";
        if (strcmp(last_dot, ".png") == 0)
            return "image/png";
        if (strcmp(last_dot, ".pdf") == 0)
            return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0)
            return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0)
            return "text/plain";
    }
    return "application/octet-stream";
}
std::string birng_content(std::vector<Request> req, int reciver)
{
    std::string target;
    for (int i = 0; i < req.size(); i++)
        if (req[i]._fd == reciver)
            target = req[i].target;
    target = target.substr(1);
    if (target == "")
        return "";
    target = get_content_type(target.c_str());

    return target;
}
std::string generateDirectoryListing(const std::string &directoryPath)
{
    std::stringstream htmlStream;
    htmlStream << "<html><body>\n";
    htmlStream << "<h1>Directory Listing: " << directoryPath << "</h1>\n";

    DIR *dir = opendir(directoryPath.c_str());
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            std::string entryName = entry->d_name;
            if (entryName != "." && entryName != "..")
            {
                htmlStream << "<p><a href=\"" << entryName << "\">" << entryName << "</a></p>\n";
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
int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "configue file needed" << std::endl;
        return 0;
    }
    int serv_fd;
    std::vector<Request> requests;

    struct sockaddr_in seraddr, client_addr;
    int fd_ready;
    std::vector<Server> servers = mainf(ac, av);
    if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("failed to create socket");
    int reuse = 1;
    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        err("setsockopt SO_REUSEADDR failed");
    fill_ser_Add(&seraddr, servers);
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
    struct epoll_event events[1];
    socklen_t len_cle = sizeof(client_addr);
    std::vector<int> client_fds;
    while (1)
    {
        fd_ready = epoll_wait(fd_epo, events, 1, -1);
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
                if (events[i].events & EPOLLIN) ////////request
                {
                    int fd, n_read;
                    off_t file_size;
                    std::string request = "";
                    char rec[LINE + 1];
                    while ((n_read = read(events[i].data.fd, rec, LINE + 1)) > 0)
                    {

                        request += rec;
                        if (rec[n_read - 1] == '\n')
                            break;
                        memset(rec, 0, LINE);
                    }
                    if (n_read == 0)
                    {
                        close(events[i].data.fd);
                        epoll_ctl(fd_epo, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        std::cout << "  client close the connction   " << std::endl;
                        continue;
                    }

                    else if (n_read < 0)
                        err("read");
                    // mahdi request
                    if (n_read > 0)
                    {
                        Request obj(request, events[i].data.fd, servers[0]);
                        requests.push_back(obj);
                        ev.events = EPOLLOUT;
                        ev.data.fd = events[i].data.fd;
                        epoll_ctl(fd_epo, EPOLL_CTL_MOD, events[i].data.fd, &ev);
                        break;
                    }
                }
                else if (events[i].events & EPOLLOUT)
                {
                    int reciver = -1;
                    std::string target;
                    int requestIndex = -1; 
                    for (int j = 0; j < requests.size(); j++)
                    {
                        if (requests[j]._fd == events[i].data.fd)
                        {
                            reciver = requests[j]._fd;
                            target = requests[j].target.substr(1);
                            requestIndex = j;
                            break;
                        }
                    }
                    std::string dir;
                    std::string content_type = birng_content(requests, reciver);
                    off_t file_size;
                    std::string response_header;
                    if (content_type == "")
                    {
                        dir = generateDirectoryListing("directorie");
                        file_size = dir.size();
                        response_header = constructResponseHeader1("text/html", file_size, requests);
                        write(events[i].data.fd, response_header.c_str(), response_header.size());
                        const size_t buffer_size = 1024;
                        char buffer[buffer_size];
                        ssize_t bytes_read;
                        ssize_t bytes_sent = write(events[i].data.fd, dir.c_str(), dir.size());
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
                        response_header = constructResponseHeader(content_type, file_size, requests);
                        write(events[i].data.fd, response_header.c_str(), response_header.size());
                        const size_t buffer_size = 1024;
                        char buffer[buffer_size];
                        ssize_t bytes_read;

                        while ((bytes_read = read(fd_file, buffer, buffer_size)) > 0)
                        {
                            std::stringstream chunk_size_stream;
                            chunk_size_stream << std::hex << bytes_read << "\r\n";
                            std::string chunk_size_line = chunk_size_stream.str();
                            write(events[i].data.fd, chunk_size_line.c_str(), chunk_size_line.size());
                            ssize_t bytes_sent = write(events[i].data.fd, buffer, bytes_read);
                            if (bytes_sent == -1)
                            {
                                err("Error sending data1");
                            }
                            write(events[i].data.fd, "\r\n", 2); 
                        }
                        write(events[i].data.fd, "0\r\n\r\n", 5);
                        close(fd_file);
                    }
                    epoll_ctl(fd_epo, EPOLL_CTL_DEL, events[i].data.fd, NULL);

                    
                    // close
                }
            }
        }
    }
}