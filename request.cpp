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
        if(str[i] != 0)
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


void fill_type(std::string &meth, std::string &tar, std::string &http, std::vector<std::string> &myreq, int *fil)
{
    std::vector<std::string>::iterator it = myreq.begin();
    std::vector<std::string> first_line;

    //split_rquest(first_line, *it, ' ');
    ft_split(*it, " ", first_line);
    try{
        meth = first_line.at(0);
    }
    catch (...){
        meth = "";
        *fil = 1;
    }
    try{
        tar = first_line.at(1);
    }
    catch (...){
        tar = "";
        *fil = 1;
    }
    try{
        http = first_line.at(2);
    }
    catch (...){
        http = "";
        *fil = 1;
    }
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
            return it->second;
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
        if(!flag2)
            tar += uri[j];
        if(j < uri.size() - 1)
            tar += "/";
        }
}

int check_path(std::string &target, std::string &stat)
{
    std::ifstream uri(target.c_str());

    if(!uri.good())
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
        else if(target[0] == '/')
            target = target.substr(1);
        if(access(target.c_str(), F_OK))
        {
            if (count_slash(target) == 0 && target != "/")
                short_uri(target, serv, &flag);
            else if (count_slash(target) >= 1)
                long_uri(target, serv, &flag);
            if (!flag)
                target = serv.root;
        }

        
    }
    if (find_key("Content-Length", postReq))
    {
        std::string val = valueOfkey("Content-Length", postReq);
        int content = std::atoi(val.c_str());

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
    std::string first;
    std::string second;

    it++;
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

void fill_post(std::vector<std::pair<std::string, std::string> > &postReq, std::vector<std::string> &post, std::vector<std::string> &bod)
{
    if (post.size() < 2)
        error("in post");
    std::vector<std::string> first_part;

    ft_split(post[0], "\r\n", first_part);
    fill_post_headers(postReq, first_part);

    for (int i = 1; i < post.size(); i++)
        bod.push_back(post[i]);
}

void get_post_status(std::string &stat, std::vector<std::pair<std::string, std::string> > &postReq)
{
    std::cout << "--------------> "<< valueOfkey("Content-Type",postReq) << std::endl;
    if (find_key("Transfer-Encoding", postReq) && valueOfkey("Transfer-Encoding",postReq) == " chunked" && 
            find_key("Content-Type", postReq) && valueOfkey("Content-Type",postReq).find("boundary") != std::string::npos)
            stat = "Chunked/boundary";
    else if (find_key("Transfer-Encoding", postReq) && valueOfkey("Transfer-Encoding",postReq) == " chunked")
        stat = "chunked";
    else if (find_key("Content-Type", postReq) && valueOfkey("Content-Type",postReq).find("boundary") != std::string::npos)
        stat = "boundary";

    else
        stat = "Bainary/Row";
}


Request::Request(std::string req, Server server)
{
    int filmap = 0;
    std::vector<std::string> myreq;
    this->status = "200 OK";
    this->header_flag = 0;
    this->fd_file = -1;
    std::cout << req << std::endl;
    this->endOfrequest = 0;
    ft_split(req, "\r\n", myreq);
    fill_type(method, target, httpVersion, myreq, &filmap);

    if(filmap)
    {
        status = "400";
        error("Bad Request");
    }
    if (method == "GET" && !filmap)
        fill_map(myRequest, myreq);
    else if (method == "POST" && !filmap)
    {

        std::vector<std::string> post;
        ft_split(req, "\r\n\r\n", post);
        fill_post(postReq, post, Body);
    }
    Request::error_handling(server);

    get_post_status(Post_status, postReq); 
}

Request::~Request()
{
}