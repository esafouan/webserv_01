#include "Request.hpp"


void Request::fill_error_pages_map()
{
    pages.insert(std::pair<std::string, std::string>("400", "error/400.html"));
    pages.insert(std::pair<std::string, std::string>("401", "error/401.html"));
    pages.insert(std::pair<std::string, std::string>("403", "error/403.html"));
    pages.insert(std::pair<std::string, std::string>("404", "error/404.html"));
    pages.insert(std::pair<std::string, std::string>("500", "error/500.html"));
    pages.insert(std::pair<std::string, std::string>("501", "error/501.html"));
    pages.insert(std::pair<std::string, std::string>("503", "error/503.html"));
    pages.insert(std::pair<std::string, std::string>("414", "error/414.html"));
}

void Request::fill_extensions_map()
{
    extensions.insert(std::pair<std::string, std::string>("text/css",".css"));
    extensions.insert(std::pair<std::string, std::string>("video/mp4",".mp4"));
    extensions.insert(std::pair<std::string, std::string>("text/csv",".csv"));
    extensions.insert(std::pair<std::string, std::string>("image/gif",".gif"));
    extensions.insert(std::pair<std::string, std::string>("text/html", ".html"));
    extensions.insert(std::pair<std::string, std::string>("image/x-icon",".ico"));
    extensions.insert(std::pair<std::string, std::string>("image/jpeg",".jpeg"));
    extensions.insert(std::pair<std::string, std::string>("application/javascript",".js"));
    extensions.insert(std::pair<std::string, std::string>("application/json",".json"));
    extensions.insert(std::pair<std::string, std::string>("image/png",".png"));
    extensions.insert(std::pair<std::string, std::string>("application/pdf", ".pdf"));
    extensions.insert(std::pair<std::string, std::string>("image/svg+xml",".svg"));
    extensions.insert(std::pair<std::string, std::string>("text/plain", ".txt"));
}

void Request::fill_headers()
{
    for (int i = 1; i < this->myHeaders.size();i++)
    {
        std::vector<std::string> line;
        if (this->myHeaders[i][0] != 0)
        {
            split_rquest_v2(line, this->myHeaders[i], ':');
            if (line.size() != 2)
                this->status = "400";
            else
                this->StoreHeaders.push_back(std::make_pair(line[0], line[1].erase(0, 1)));
        }
    }
}

void Request::fill_query()
{
    size_t pos = target.find("?");

    if (pos != target.npos)
    {
        query = "QUERY_STRING=";
        query += target.substr(pos + 1);
        target = target.substr(0, pos);
    }
}

void      Request::fill_method_type()
{
    std::vector<std::string> first_line;

    ft_split(this->myHeaders[0], " ", first_line);
    if (first_line.size() != 3)
        this->status = "400";
    this->method = first_line[0];
    this->target = first_line[1];
    this->httpVersion = first_line[2];
}
