#include "multi.hpp"

Response::Response(int f, std::map<int, Request> &r) : client_fd(f), req(r)
{
}
int  Response::get_client_fd()
{
    return this->client_fd;
}
bool Response::is_get()
{
    if ((req[client_fd].method == "GET"))
        return 1;
    return 0;
}
bool Response::is_cgi()
{
    if ((req[client_fd].target.find(".php")) != std::string::npos || (req[client_fd].target.find(".py")) != std::string::npos)
        return 1;
    return 0;
}
bool Response::is_delete()
{
    if ((req[client_fd].method == "DELETE"))
        return 1;
    return 0;
}
bool Response::is_error()
{
    if ((req[client_fd].status != "201" && req[client_fd].status != "200" && req[client_fd].status != "301"))
        return 1;
    return 0;
}
bool Response::is_redirection()
{
    if ((req[client_fd].status == "301"))
        return 1;
    return 0;
}
bool Response::is_post()
{
    if (req[client_fd].endOfrequest == 0 && (req[client_fd].method == "POST"))
        return 1;
    return 0;
}
std::string Response::normal_pages_header1(size_t contentLength)
{
    std::string header;

    header += "HTTP/1.1 ";
    header += req[client_fd].status;
    header += "\r\n";
    header += "Content-Type: ";
    if(req[client_fd].target.find("data.py") != std::string::npos)
        header += req[client_fd].content_type_python + "\r\n";
    else
        header += "text/html\r\n";
    std::stringstream contentLengthStream;
    contentLengthStream << contentLength;
    header += "Content-Length: " + contentLengthStream.str() + "\r\n";
    header += "\r\n";

    return header;
}
std::string Response::normal_pages_header(size_t contentLength)
{
    std::string header;

    header += "HTTP/1.1 ";
    header += req[client_fd].status;
    header += "\r\n";
    header += "Content-Type: text/html\r\n";
    std::stringstream contentLengthStream;
    contentLengthStream << contentLength;
    header += "Content-Length: " + contentLengthStream.str() + "\r\n";
    header += "\r\n";

    return header;
}
std::string Response::get_last()
{
    std::string path = req[client_fd].target;
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
std::string Response::redirect_pages_header()
{
    std::string header;
    header += "HTTP/1.1 ";
    header += req[client_fd].status;
    header += "\r\n";
    header += "Location: ";
    header += get_last();
    header += "\r\n";
    header += "\r\n";


    return header;
}
void Response::response_by_a_page(std::string path)
{
    size_t file_size;

    std::ifstream fd_file(path.c_str());
    fd_file.seekg(0, std::ios::end);
    std::streampos fileSize = fd_file.tellg();
    fd_file.seekg(0, std::ios::beg);
    std::string response_header = normal_pages_header((size_t)fileSize);
    write(client_fd, response_header.c_str(), response_header.size());
    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    if (fd_file.read(buffer, (size_t)fileSize))
    {
        ssize_t bytes_sent = write(client_fd, buffer, (size_t)fileSize);
        if (bytes_sent == -1)
        {
            std::remove(req[client_fd].outfile_name.c_str());
        }
    }
    else
        perror("ss : ");
    fd_file.close();
}
std::string Response::get_content_type()
{
    const char *path = req[client_fd].target.c_str();
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
        if (strcmp(last_dot, ".py") == 0)
            return "application/python";
        if (strcmp(last_dot, ".pyon") == 0)
            return "application/json";
        if (strcmp(last_dot, ".png") == 0)
            return "image/png";
        if (strcmp(last_dot, ".pdf") == 0)
            return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0)
            return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0)
            return "text/plain";
        if (strcmp(last_dot, ".php") == 0)
            return "application/x-httpd-php";
    }
    return "";
}
std::string Response::generateDirectoryListing()
{
    std::stringstream htmlStream;
    htmlStream << "<html><body>\n";
    htmlStream << "<h1>Directory Listing: " << req[client_fd].target << "</h1>\n";
    DIR *dir = opendir(req[client_fd].target.c_str());

    if (dir)
    {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            std::string entryName = entry->d_name;
        
            if (entryName != "." && entryName != "..")
            {
                if(req[client_fd].flag_uri == 1)
                {
                    
                    htmlStream << "<p><a href=\"" << req[client_fd].uri_for_response + "/" << entryName << "\">" << entryName << "</a></p>\n";
                } 
                else 
                {
                    struct stat fileStat;
                    if (stat((req[client_fd].target + entryName).c_str(), &fileStat) == 0)
                    {
                        if (S_ISDIR(fileStat.st_mode))
                            entryName += "/";
                    }
                    htmlStream << "<p><a href=\"" << entryName << "\">" << entryName << "</a></p>\n";
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
int Response::directorie_list()
{
    std::string dir;
    off_t file_size;
    std::string response_header;
    std::string target = req[client_fd].target;
    if (target[0] == '/')
        target = target.substr(1);
    dir = generateDirectoryListing();
    file_size = dir.size();
    response_header = normal_pages_header((size_t)(file_size));
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
std::string Response::chunked_header()
{
    std::string header;

    header += "HTTP/1.1 ";
    header += req[client_fd].status;
    header += "\r\n";

    // Content-Type header
    header += "Content-Type: " + get_content_type() + "\r\n";
    // chunks
    header += "Transfer-Encoding: chunked\r\n";

    // Blank line
    header += "\r\n";

    return header;
}
int Response::chunked_response_headers()
{
    std::string target = req[client_fd].target;
    if (target[0] == '/')
        target = target.substr(1);
    req[client_fd].fd_file.seekg(0, std::ios::end);
    std::streampos fileSize = req[client_fd].fd_file.tellg();
    req[client_fd].fd_file.seekg(0, std::ios::beg);
    req[client_fd].chunked_file_size_response = (size_t)fileSize;
    send(client_fd, chunked_header().c_str(), chunked_header().size(), 0);
    req[client_fd].header_flag = 1;
    return 1;
}
int Response::chunked_response_body()
{

    size_t buffer_size = 1024;
    char buffer[buffer_size];
    if (req[client_fd].chunked_file_size_response > 1024)
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
    if ((req[client_fd].fd_file.read(buffer, buffer_size)))
    {
        std::stringstream chunk_size_stream;
        chunk_size_stream << std::hex << buffer_size << "\r\n";
        std::string chunk_size_line = chunk_size_stream.str();
        chunk_size_line.append(buffer, buffer_size);
        chunk_size_line.append("\r\n", 2);
        if (send(client_fd, chunk_size_line.c_str(), chunk_size_line.size(), 0) < 0)
        {
            req[client_fd].fd_file.close();
            return 0;
        }
        return 1;
    }
    return 0;
}
