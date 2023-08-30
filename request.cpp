#include "multi.hpp"
#include <cstdlib>
#include <stdio.h>

void Request::print_element()
{
    std::cout << method << std::endl;
    std::cout << target << std::endl;
    std::cout << httpVersion << std::endl;

    std::map<std::string, std::string>::const_iterator it = myRequest.begin();
    for (it; it != myRequest.end(); it++)
    {
        std::cout << "key = " << it->first;
        std::cout << " value =" << it->second << std::endl;
    }
}

void split_rquest(std::vector<std::string> &r, std::string &req, char c)
{
    std::string tmp;
    std::istringstream tokensOfreq(req);

    while (std::getline(tokensOfreq, tmp, c))
    {
        r.push_back(tmp);
    }
}
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

void split_rquest_v2(std::vector<std::string> &r, std::string &req, char c)
{
    std::string tmp;
    std::istringstream tokensOfreq(req);

    std::getline(tokensOfreq, tmp, c);
    r.push_back(tmp);
    std::getline(tokensOfreq, tmp, '\n');
    r.push_back(tmp);
}

void ignore_whitespaces(std::string &str)
{
    size_t begin = str.find_first_not_of(" \t\r\n");

    if (begin != std::string::npos)
        str.substr(begin);
}

void fill_type(std::string &meth, std::string &tar, std::string &http, std::vector<std::string> &myreq, int *fil)
{
    std::vector<std::string>::iterator it = myreq.begin();
    std::vector<std::string> first_line;

    split_rquest(first_line, *it, ' ');

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

void fill_map(std::map<std::string, std::string> &map, std::vector<std::string> &myreq)
{
    std::vector<std::string>::iterator it = myreq.begin();
    it++;
    std::string first;
    std::string second;
    for (; it != myreq.end(); it++)
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
            map.insert(std::pair<std::string, std::string>(first, second));
        }
    }
}

void error(std::string str)
{
    std::cout << str << std::endl;
    exit(1);
}

int find_key(std::string key, std::map<std::string, std::string> &headers)
{
    std::map<std::string, std::string>::iterator it = headers.find(key);

    if (it != headers.end())
        return 1;
    return 0;
}

std::string valueOfkey(std::string key, std::map<std::string, std::string> &headers)
{
    return (headers[key]);
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
            return key;
    }
    return "";
}

void pars_headers(std::map<std::string, std::string> &headers, std::string &stat)
{
    std::map<std::string, std::string>::iterator it = headers.begin();

    if (find_key("Transfer-Encoding", headers) && valueOfkey("Transfer-Encoding", headers) != " chunked")
    {
        stat = "501";
        error("501");
    }
}

int check_uri(std::string &uri)
{
    std::string forbiden = " \"<>#%{}\\^~[]`/?&=+$,|";
    int i = 0;
    int j;
    while (uri[i++])
    {
        if (forbiden.find(uri[i]) != std::string::npos)
            return 0;
    }
    return 1;
}

void replace_slash_in_target(Server &serv, std::string &targ, int *flag)
{
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME == "/")
        {
            targ = serv.locations[i].root.append(serv.locations[i].NAME) + targ.substr(1);
            *flag = 1;
        }
    }
}

void replacing(std::string &line, std::string toSearch, std::string toReplace)
{
    std::string content;
    size_t pos = 0;
    size_t i = 0;
    size_t j;
    size_t next = 0;

    while (line[i])
    {
        pos = line.find(toSearch, next);
        if (i == pos && line[i])
        {
            j = 0;
            while (j < toReplace.length())
            {
                content += toReplace[j];
                j++;
            }
            i += toSearch.length();
            next = i;
        }
        else
        {
            content += line[i];
            i++;
        }
    }
    line = content;
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
    //std::cout << "tar = " << tar << std::endl;

    for (int i = 0; i < serv.locations.size(); i++)
    {   if(serv.locations[i].NAME != "/")
        {
            size_t pos = tar.find(serv.locations[i].NAME);
            if (pos != std::string::npos)
            {
                tar = serv.locations[i].root;
                *flag = 1;
            }
        }
    }
     // std::cout << "target = " << tar << std::endl;
}

void long_uri(std::string &tar, Server &serv, int *flag)
{
    std::vector<std::string> uri;

    ft_split(tar, "/", uri);
    tar = "";
    for (int j = 0; j < uri.size(); j++)
    {
        for (int i = 0; i < serv.locations.size(); i++)
        {
            if (uri[j] == serv.locations[i].NAME)
            {
                //if(j < uri.size() - 1)
                //    tar += serv.locations[i].root + "/";
               // else
                    tar += serv.locations[i].root;
                //uri[i] = serv.locations[i].root;
                *flag = 1;
            }
        }
    }
}

void Request::error_handling(Server &serv)
{
    if ((method == "GET" && serv.locations[0].GET == false) || (method == "POST" && serv.locations[0].POST == false) || (method == "DELETE" && serv.locations[0].DELETE == false))
    {
        status = "405";
        target = "directorie/errorpages/404.html";
        //error("405 Method Not Allowed");
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
    // else if (!check_uri(target))
    // {
    //    status = "400";
    //    error ("400 Bad Request");
    // }
    else
    {
        int flag = 0;
        if (target == "/")
            replace_slash_in_target(serv, target, &flag);
        else if (count_slash(target) == 1 && target != "/")
            short_uri(target, serv, &flag);
        else if (count_slash(target) > 1)
            long_uri(target, serv, &flag);
        if (!flag)
            target = serv.root;
        
    }
    if (find_key("Content-Length", postReq))
    {
        std::string val = valueOfkey("Content-Length", postReq);
        int content = std::atoi(val.c_str());

        //std::cout << content << std::endl;
        //std::cout << serv.max_body << std::endl;

        if (content > serv.max_body)
        {
            status = "413";
            error("413 Request Entity Too Large");
        }
    }
    else if (httpVersion != "HTTP/1.1")
    {
        status = "400";
        error("400 Bad Request");
    }
    pars_headers(myRequest, status);
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
    // this->_fd = req._fd;
    this->myRequest = req.myRequest;
    this->fd_file = req.fd_file;

    return (*this);
}

Request::Request()
{
    this->header_flag = 0;
    this->fd_file = -1;
}

void fill_post_headers(std::vector<std::pair<std::string, std::string> > &postReq, std::vector<std::string> &myreq)
{
    std::vector<std::string>::iterator it = myreq.begin();
    it++;
    std::string first;
    std::string second;
    for (; it != myreq.end(); it++)
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
            postReq.push_back(std::make_pair(first, second));
        }
    }
}

void fill_post(std::vector<std::pair<std::string, std::string> > &postReq, std::vector<std::string> &post, std::string &bod)
{
    if (post.size() > 2)
    {
        error("in post");
    }
    std::vector<std::string> first_part;
    ft_split(post[0], "\r\n", first_part);
    fill_post_headers(postReq, first_part);
    if (post.size() == 2)
        bod = post[1];
}

void get_post_status(std::string &stat, std::vector<std::pair<std::string, std::string> > &postReq)
{
    if (find_key("Transfer_encoding", postReq) && valueOfkey("Transfer_encoding",postReq) == " chunked")
        stat = "chunked";
    else if (find_key("Content_Type", postReq) && valueOfkey("Content_Type",postReq) == " Boundray")
        stat = "Boundary";
    else if (find_key("Transfer_encoding", postReq) && valueOfkey("Transfer_encoding",postReq) == " chunked" && 
            find_key("Content_Type", postReq) && valueOfkey("Content_Type",postReq) == " Boundray")
            stat = "Chunked/Boundary";
    else
        stat = "Bainary/Row";
}

Request::Request(std::string req, Server server)
{
    int filmap = 0;
    std::vector<std::string> myreq;
    //std::cout << req << std::endl;
    this->status = "200 OK";
    this->header_flag = 0;
    this->fd_file = -1;
    Body = "";

   
    ft_split(req, "\r\n", myreq);
    fill_type(method, target, httpVersion, myreq, &filmap);
    if (method == "GET" && !filmap)
        fill_map(myRequest, myreq);
    else if (method == "POST" && !filmap)
    {

        std::vector<std::string> post;
        ft_split(req, "\r\n\r\n", post);
        fill_post(postReq, post, Body);
    }
    // std::cout << Body << std::endl;
    // Request::print_element();
    Request::error_handling(server);
    get_post_status(Post_status, postReq);
    
    for(int i = 0;i < postReq.size(); i++)
        std::cout << postReq[i].first << " -//- "<< postReq[i].second << std::endl;
    
    std::cout << Post_status << std::endl;


   // std::cout << "final = " << target << std::endl;
}

Request::~Request()
{
}