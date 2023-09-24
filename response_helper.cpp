#include "multi.hpp"

int response_header(std::string target, int client_fd, std::map<int, Request> &req)
{
    std::string target_to_send = get_content_type(target.c_str());
    if (target[0] == '/')
        target = target.substr(1);

    req[client_fd].fd_file.seekg(0, std::ios::end);
    // Get the position, which is the size of the file
    std::streampos fileSize = req[client_fd].fd_file.tellg();
    req[client_fd].fd_file.seekg(0, std::ios::beg);
    req[client_fd].chunked_file_size_response = (size_t)fileSize;

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
#include <iostream>
#include <cstring>
int chunked_response(std::string target, int client_fd, std::map<int, Request> &req)
{

    size_t buffer_size = 1024;
    char buffer[buffer_size];

    if (req[client_fd].chunked_file_size_response > 1024 )
        req[client_fd].chunked_file_size_response -= 1024;
    else if (req[client_fd].chunked_file_size_response > 0)
    {
        buffer_size = req[client_fd].chunked_file_size_response;
        req[client_fd].chunked_file_size_response = 0;
    }
    else
    {
        write(client_fd, "0\r\n\r\n", 5);
        req[client_fd].fd_file.close();
        return 0;
    }
    if ((req[client_fd].fd_file.read(buffer, buffer_size)) )
    {
        std::stringstream chunk_size_stream;
        chunk_size_stream << std::hex << buffer_size << "\r\n";
        std::string chunk_size_line = chunk_size_stream.str();
        chunk_size_line.append(buffer,buffer_size);
        chunk_size_line.append("\r\n",2);
        if (send(client_fd, chunk_size_line.c_str(), chunk_size_line.size(), 0) < 0)
        {
            // perror("chunksize ");
            req[client_fd].fd_file.close();
            return 0;
        }
        return 1;
    }

    

    return 0;
}