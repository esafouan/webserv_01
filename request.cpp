#include "multi.hpp"
#include <cstdlib>
#include <stdio.h>

void Request::print_element()
{
    std::cout << method <<std::endl;
    std::cout << target <<std::endl;
    std::cout << httpVersion <<std::endl;

    std::map<std::string,std::string>::const_iterator it = myRequest.begin();
    for (it ;it != myRequest.end(); it++)
    {
       std::cout << "key = " << it->first;
       std::cout << " value =" << it->second << std::endl;
    }
}

void split_rquest(std::vector<std::string> & r, std::string & req, char c)
{
    std::string                 tmp;
    std::istringstream          tokensOfreq(req);

    while(std::getline(tokensOfreq, tmp, c))
    {
        r.push_back(tmp);
    }
}

void split_rquest_v2(std::vector<std::string> & r, std::string & req, char c)
{
    std::string                 tmp;
    std::istringstream          tokensOfreq(req);

    std::getline(tokensOfreq, tmp, c);
    r.push_back(tmp);
    std::getline(tokensOfreq, tmp, '\n');
    r.push_back(tmp);
}

void ignore_whitespaces(std::string & str)
{
    size_t begin = str.find_first_not_of(" \t\r\n");

    if (begin != std::string::npos) 
       str.substr(begin);
}

void fill_type(std::string & meth, std::string & tar, std::string & http, std::vector<std::string> &myreq, int *fil)
{
    std::vector<std::string>::iterator it = myreq.begin();
    std::vector<std::string> first_line;

    split_rquest(first_line, *it, ' ');

    try
    {
        meth = first_line.at(0);
    }
    catch(...){
        meth = "";
        *fil = 1;
    }

    try
    {
        tar = first_line.at(1);
    }
    catch(...){
        tar = "";
        *fil = 1;
    }

    try
    {
        http = first_line.at(2);
    }
    catch(...){
        http = "";
        *fil = 1;
    }
    

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

void fill_map(std::map<std::string, std::string> &map, std::vector<std::string> &myreq)
{
    std::vector<std::string>::iterator it = myreq.begin();
    it++;
    std::string first;
    std::string second;
    for(; it != myreq.end(); it++)
    {
        std::vector<std::string> line;
        if(check_new_lines(*it) != 0)
        {
            split_rquest_v2(line, *it, ':');
            try
            {
                first = line.at(0);
                second = line.at(1); 
            }
            catch(...)
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
    exit (1);
}

int find_key(std::string key, std::map<std::string, std::string> &headers)
{
    std::map<std::string, std::string>::iterator it = headers.find(key);

    if(it != headers.end())
        return 1;
    return 0;
}

std::string valueOfkey(std::string key, std::map<std::string, std::string> &headers)
{
    return (headers[key]);
}

void pars_headers(std::map<std::string, std::string> &headers, std::string & stat)
{
    std::map<std::string, std::string>::iterator it = headers.begin();


    if (!find_key("Transfer-Encoding", headers) && valueOfkey("Transfer-Encoding", headers) != " chunked")
    {
        stat = "501";
        error("501");
    }

    else if (!find_key("Transfer-Encoding", headers) && !find_key("Content-Length", headers))
    {
        stat = "400";
        error("400");
    }


}

int check_uri(std::string &uri)
{
    std::string forbiden = " \"<>#%{}\\^~[]`/?&=+$,|";
    int i = 0;
    int j;
    while(uri[i++])
    {
        if(forbiden.find(uri[i]) != std::string::npos)
            return 0;
    }
    return 1;
}

void Request::error_handling(Server &serv)
{
    //std::cout << serv.locations[0].NAME << std::endl;
    if (( method == "GET" && serv.locations[0].GET == false )
        || ( method == "POST" && serv.locations[0].POST == false )
        || ( method == "DELETE" && serv.locations[0].DELETE == false ))
    {
        status = "404";
        error ("404 Bad Request");
    }
    else if (method != "GET" && method != "DELETE" && method != "POST")
    {
        if(method == "PUT" || method == "HEAD" || method == "TRACE" || method == "CONNECT")
        {
            status = "501";
            error ("501 not implemented");
        }
        status = "404";
        error ("404 Bad Request");
    }
    else if (target.size() > 2048)
    {
        status = "414";
        error ("414 Request-URI Too Long");
    }
    else if (!check_uri(target))
    {
       status = "400";
       error ("400 Bad Request");
    }
    else 
    {
        int flag = 0;
        for(int i = 0; i < serv.locations.size();i++)
        {
            //std::cout << serv.locations[i].NAME << std::endl;
            //std::cout << target << std::endl;
            if (serv.locations[i].NAME == target)
            {
                  target = serv.locations[i].root;
                  flag = 1;
                  break;
            }   
        }
        if(!flag)
        {
            status = "404";
            error ("404 Not Found");
        }
    }

    if(find_key("Content-Length", myRequest))
    {
        std::string val = valueOfkey("Content-Length", myRequest);
       int content = std::atoi(val.c_str());
       if(content > serv.max_body)
        {
           status = "413";
           error("413 Request Entity Too Large");
       }
    }
    //else if(httpVersion != "HTTP/1.1")
    //{
        //std::cout << httpVersion << "-" << std::endl;
    //   status = "400";
    //   error("400 Bad Request");
   // }

    //pars_headers(myRequest, status);
}

Request::Request(Request const &req)
{
    *this = req;
}

Request& Request::operator=(Request const & req)
{
        this->method = req.method;
        this->target = req.target;
        this->httpVersion = req.httpVersion;
        this->status = req.status;
        // this->_fd = req._fd;
        this->myRequest = req.myRequest;

        return(*this);
}

Request::Request()
{

}

Request::Request(std::string req, Server server)
{
    int filmap = 0;
    std::vector<std::string> myreq;
    this->status = "200 OK";
    split_rquest(myreq, req, '\n');
    fill_type(method, target, httpVersion, myreq, &filmap);
    if (!filmap)
        fill_map(myRequest, myreq);
    // Request::print_element();
    Request::error_handling(server);
//    std::cout << target << std::endl;
    //print_elements
}

Request::~Request()
{
}