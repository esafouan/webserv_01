#include "multi.hpp"
#include <cstdlib>
#include <stdio.h>

void ft_split(std::string input, std::string delimiter, std::vector<std::string> &parts)
{
    size_t startPos = 0;
    size_t endPos;

    while ((endPos = input.find(delimiter, startPos)) != std::string::npos)
    {
        parts.push_back(input.substr(startPos, endPos - startPos));
        startPos = endPos + delimiter.length();
    }

    // Add the last part
    parts.push_back(input.substr(startPos));
}

int check_new_lines(std::string str)
{
    int i = 0;
    while (str[i++])
    {
        if (str[i] != 0)
            return 1;
    }
    return 0;
}

void split_rquest_v2(std::vector<std::string> &r, std::string &req, char c)
{
    std::string tmp;
    std::istringstream tokensOfreq(req);

    std::getline(tokensOfreq, tmp, c);
    r.push_back(tmp);
    std::getline(tokensOfreq, tmp, '\n');
    r.push_back(tmp);
}

void fill_type(std::string &meth, std::string &tar, std::string &http, std::vector<std::string> &myHeaders, int *fil)
{
    std::vector<std::string>::iterator it = myHeaders.begin();
    std::vector<std::string> first_line;

    ft_split(*it, " ", first_line);
    try
    {
        meth = first_line.at(0);
    }
    catch (...)
    {
        meth = "";
        *fil = 1;
    }
    try
    {
        tar = first_line.at(1);
    }
    catch (...)
    {
        tar = "";
        *fil = 1;
    }
    try
    {
        http = first_line.at(2);
    }
    catch (...)
    {
        http = "";
        *fil = 1;
    }
}

void error(std::string str)
{
    std::cout << str << std::endl;
    exit(1);
}

int find_key(std::string key, std::vector<std::pair<std::string, std::string> > &postR)
{
    std::vector<std::pair<std::string, std::string> >::iterator it = postR.begin();

    for (it; it != postR.end(); it++)
    {
        if (it->first == key)
            return 1;
    }
    return 0;
}

std::string valueOfkey(std::string key, std::vector<std::pair<std::string, std::string> > &postR)
{
    std::vector<std::pair<std::string, std::string> >::iterator it = postR.begin();

    for (it; it != postR.end(); it++)
    {
        if (it->first == key)
            return it->second;
    }
    return "";
}

void pars_headers(std::vector<std::pair<std::string, std::string> > &headers, std::string &stat)
{
    if (find_key("Transfer-Encoding", headers) && valueOfkey("Transfer-Encoding", headers) != "chunked")
    {
        stat = "501";
        error("501");
    }
}

void replace_slash_in_target(Server &serv, std::string &targ, int *flag)
{
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME == "/")
        {
            targ = serv.locations[i].NAME.append(serv.locations[i].root);
            *flag = 1;
        }
    }
}

int count_slash(std::string tar)
{
    int count = 0;
    for (int i = 0; i < tar.length(); i++)
        if (tar[i] == '/')
            count++;
    return (count);
}

void short_uri(std::string &tar, Server &serv, int *flag)
{
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME != "/")
        {
            size_t pos = tar.find(serv.locations[i].NAME);
            if (pos != std::string::npos)
            {
                tar = serv.locations[i].root;
                *flag = 1;
            }
        }
    }
}

void long_uri(std::string &tar, Server &serv, int *flag)
{
    std::vector<std::string> uri;

    ft_split(tar, "/", uri);
    tar = "";
    int flag2;
    for (int j = 0; j < uri.size(); j++)
    {
        flag2 = 0;
        for (int i = 0; i < serv.locations.size(); i++)
        {
            if (uri[j] == serv.locations[i].NAME)
            {
                tar += serv.locations[i].root;
                *flag = 1;
                flag2 = 1;
            }
        }
        if (!flag2)
            tar += uri[j];
        if (j < uri.size() - 1)
            tar += "/";
    }
}

int check_path(std::string &target, std::string &stat)
{
    std::ifstream uri(target.c_str());

    if (!uri.good())
        return 0;
    return 1;
}

void Request::error_handling(Server &serv)
{
    if ((method == "GET" && serv.locations[0].GET == false) || (method == "POST" && serv.locations[0].POST == false) || (method == "DELETE" && serv.locations[0].DELETE == false))
    {
        status = "405";
        error("405 Method Not Allowed");
    }
    else if (method != "GET" && method != "DELETE" && method != "POST")
    {
        if (method == "PUT" || method == "HEAD" || method == "TRACE" || method == "CONNECT")
        {
            status = "501";
            error("501 not implemented");
        }
        status = "404";
        error("404 Bad Request");
    }
    else if (target.size() > 2048)
    {
        status = "414";
        error("414 Request-URI Too Long");
    }
    else
    {
        int flag = 0;

        if (target == "/")
            replace_slash_in_target(serv, target, &flag);
        else if (target[0] == '/')
            target = target.substr(1);
        if (access(target.c_str(), F_OK))
        {
            if (count_slash(target) == 0 && target != "/")
                short_uri(target, serv, &flag);
            else if (count_slash(target) >= 1)
                long_uri(target, serv, &flag);
            if (!flag)
                target = serv.root;
        }
    }
    // if (find_key("Content-Length", StoreHeaders))
    // {
    //     std::string val = valueOfkey("Content-Length", StoreHeaders);
    //     int content = std::atoi(val.c_str());

    //     if (content > serv.max_body)
    //     {
    //         status = "413";
    //         error("413 Request Entity Too Large");
    //     }
    // }
    if (httpVersion != "HTTP/1.1")
    {
        status = "400";
        error("400 Bad Request");
    }
    pars_headers(StoreHeaders, status);
}

Request::Request(Request const &req)
{
    *this = req;
}

Request &Request::operator=(Request const &req)
{
    this->method = req.method;
    this->target = req.target;
    this->httpVersion = req.httpVersion;
    this->status = req.status;
    this->header_flag = req.header_flag;
    this->fd_file = req.fd_file;
    this->endOfrequest = req.endOfrequest;
    this->lenght_Readed = req.lenght_Readed;
    this->Post_status = req.Post_status;
    this->lenght_of_content = req.lenght_of_content;
    this->extension = req.extension;
    this-> outfile_name = req.outfile_name;
    //this->outfile = req.outfile;
    this->boundary_separater = req.boundary_separater;
    this->outfile.copyfmt(req.outfile);
    this->outfile.clear();
    outfile.open(req.outfile_name.c_str());

    return (*this);
}

Request::Request()
{
    this->header_flag = 0;
    this->fd_file = -1;
}

void fill_headers(std::vector<std::pair<std::string, std::string> > &StoreHeaders, std::vector<std::string> &myHeaders)
{
    std::vector<std::string>::iterator it = myHeaders.begin();
    std::string first; 
    std::string second;

    it++;
    for (; it != myHeaders.end(); it++)
    {
        std::vector<std::string> line;
        if (check_new_lines(*it) != 0)
        {
            split_rquest_v2(line, *it, ':');
            try
            {
                first = line.at(0);
                second = line.at(1);
            }
            catch (...)
            {
                first = "";
                second = "";
                continue;
            }
            StoreHeaders.push_back(std::make_pair(first, second.erase(0, 1)));
        }
    }
}

void fill_post_body(std::vector<std::string> &myreq, std::vector<std::string> &bod)
{
    for (int i = 1; i < myreq.size(); i++)
        bod.push_back(myreq[i]);
}

std::string get_separater(std::string val)
{
    std::string sep = "";

    size_t pos = val.find("=");

    if (pos != std::string::npos)
        sep = val.substr(pos + 1);
    sep = "--" + sep ;
    return sep;
}

void Request::get_post_status()
{
    if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) == "chunked" &&
        find_key("Content-Type", StoreHeaders) && valueOfkey("Content-Type", StoreHeaders).find("boundary") != std::string::npos)
        Post_status = "Chunked/boundary";
    else if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) == "chunked")
    {
        outfile_name = get_current_time() + ".txt";
        outfile.open(outfile_name.c_str(), std::ios::binary);
        Post_status = "chunked";
    }
        
    else if (find_key("Content-Type", StoreHeaders) && valueOfkey("Content-Type", StoreHeaders).find("boundary") != std::string::npos)
    {
        outfile_name = get_current_time() + ".txt";
        outfile.open(outfile_name.c_str(), std::ios::binary);
        Post_status = "boundary";
        boundary_separater = get_separater(valueOfkey("Content-Type", StoreHeaders));
        // std::cout << "saad ->"<< boundary_separater << std::endl;
    }

    else
    {
        outfile_name = "directorie/upload/" + get_current_time() + extension;
        outfile.open(outfile_name.c_str(),  std::ios::binary);

        Post_status = "Bainary/Row";
    }
        
}

std::string generate_extention(std::string content_type)
{
    if (content_type.find("text/css") != std::string::npos)
        return (".css");
    else if (content_type.find("video/mp4") != std::string::npos)
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

Request::Request(std::string req, Server server)
{
    int filmap = 0;
    std::vector<std::string> myreq;
    std::vector<std::string> myHeaders;
    this->status = "200 OK";
    this->header_flag = 0;
    this->fd_file = -1;
    this->endOfrequest = 1;
    this->lenght_Readed = 0;

    
    ft_split(req, "\r\n", myHeaders);
    fill_type(method, target, httpVersion, myHeaders, &filmap);

    if (filmap)
    {
        status = "400";
        error("Bad Request");
    }
    fill_headers(StoreHeaders, myHeaders);
    
    // print Headers
    // for (int i = 0; i < StoreHeaders.size(); i++)
    //     std::cout << "val = " << StoreHeaders[i].first << " key = " << StoreHeaders[i].second << std::endl;
    
    Request::error_handling(server);

    if (method == "POST")
        this->endOfrequest = 0;
    
   
    this->lenght_of_content = std::atoi(valueOfkey("Content-Length", StoreHeaders).c_str());
    extension = generate_extention(valueOfkey("Content-Type", StoreHeaders));
    //std::cout << extension << std::endl;
    if (method == "POST")
        Request::get_post_status();
}

Request::~Request()
{
}