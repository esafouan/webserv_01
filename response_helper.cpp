#include "multi.hpp"

int response_header(std::string target, int client_fd, std::map<int, Request> &req)
{
    std::string target_to_send = get_content_type(target.c_str());
    //std::cout << target << std::endl;
    if(target[0] == '/')
        target = target.substr(1);
    //std::cout << target  << std::endl;
    req[client_fd].fd_file = open(target.c_str(), O_RDONLY);
    std::string response_header = constructResponseHeader(target_to_send, req[client_fd].status);
    send(client_fd, response_header.c_str(), response_header.size(), 0);
    req[client_fd].header_flag = 1;
    return 1;
}

int directorie_list(std::string target, int client_fd,  std::map<int, Request> &req)
{
    std::string dir;
    off_t file_size;
    std::string response_header;
    std::cout << "in dir ="<< target << std::endl;
    //std::cout <<"resp = " <<target << std::endl;
    if(target[0] == '/')
        target = target.substr(1);
    dir = generateDirectoryListing(target, req, client_fd);
    file_size = dir.size();
    response_header = construct_error_page("text/html", file_size,req[client_fd].status);
    write(client_fd, response_header.c_str(), response_header.size());
    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    ssize_t bytes_read;
    ssize_t bytes_sent = write(client_fd, dir.c_str(), dir.size());
    if (bytes_sent == -1)
    {
        perror("read ld ");
    }
    return 0;
}

int chunked_response(std::string target, int client_fd, std::map<int, Request> &req)
{

    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    ssize_t bytes_read;
    if ((bytes_read = read(req[client_fd].fd_file, buffer, buffer_size)) > 0)
    {
        // chunks
        std::stringstream chunk_size_stream;
        chunk_size_stream << std::hex << bytes_read << "\r\n";
        std::string chunk_size_line = chunk_size_stream.str();
        if (send(client_fd, chunk_size_line.c_str(), chunk_size_line.size(), 0) < 0)
        {
            // perror("chunksize ");
            close(req[client_fd].fd_file);
            return 0;
        }
        // chunks
        ssize_t bytes_sent = send(client_fd, buffer, bytes_read, 0);
        if (bytes_sent == -1)
        {
            // perror("in chunk ");
            close(req[client_fd].fd_file);
            return 0;
        }
        write(client_fd, "\r\n", 2);
    }
    if (bytes_read == 0)
    {

        write(client_fd, "0\r\n\r\n", 5);
        close(req[client_fd].fd_file);
        return 0;
    }
    else if (bytes_read > 0)
    {
        return 1;
    }
    return 0;
}