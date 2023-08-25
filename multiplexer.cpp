#include "multi.hpp"


void err(std::string str)
{
    std::cout << str << std::endl;
    _exit(1);
}
std::string constructResponseHeader(const std::string &contentType, size_t contentLength, std::vector<Request> req)
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
    std::vector<Server> servers = mainf(ac, av);
    soc s(servers[0]);
    s.init();
    s.run();

    
}