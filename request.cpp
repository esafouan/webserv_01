#include "multi.hpp"
#include <cstdlib>
#include <stdio.h>

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

Request::Request(std::string req,int fd) : _fd(fd)
{
    int filmap = 0;

    // std::cout << req << std::endl;
    std::vector<std::string> myreq;
    split_rquest(myreq, req, '\n');

    //std::vector<std::string>::iterator it = myreq.begin();
    //for(;it != myreq.end(); it++)
    //    std::cout << *it <<std::endl;
    fill_type(method, target, httpVersion, myreq, &filmap);


    if(!filmap)
    {
        fill_map(myRequest, myreq);  
    }
        
    // ///print_elements


}

Request::~Request()
{
}