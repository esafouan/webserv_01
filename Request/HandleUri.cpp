#include "Request.hpp"

void Request::replace_slash_in_target(Server &serv)
{
    int no_root_location = 0;

    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME == "/")
        {
            no_root_location = 1;

            if ((method == "GET" && serv.locations[i].GET == false) 
                || (method == "POST" && serv.locations[i].POST == false) 
                    || (method == "DELETE" && serv.locations[i].DELETE == false))
                        status = "405";
            
            if (serv.locations[i]._return == "")
            {
                if (serv.locations[i].index == "")
                {
                    if (serv.locations[i].autoindex == true)
                        target = serv.locations[i].root;
                    else
                        status = "403";
                }
                else
                    target = serv.locations[i].root + "/" + serv.locations[i].index;
            }
            else
            {
                target = serv.locations[i]._return;
                return ;
            }
        }
    }
    if (!no_root_location)
        target = serv.root;
}

int Request::count_slash(std::string tar)
{
    int count = 0;
    for (int i = 0; i < tar.length(); i++)
        if (tar[i] == '/')
            count++;
    return (count);
}

void Request::short_uri(Server &serv)
{
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME != "/")
        {
            if (target == serv.locations[i].NAME)
            {
                flag_uri = 1;
                if ((method == "GET" && serv.locations[i].GET == false) 
                    || (method == "POST" && serv.locations[i].POST == false) 
                    || (method == "DELETE" && serv.locations[i].DELETE == false))
                    status = "405";
                else if (serv.locations[i]._return == "")
                {
                    if (serv.locations[i].index == "")
                    {
                        if (serv.locations[i].autoindex == true)
                            target = serv.locations[i].root;
                        else
                            status = "403";
                    }
                    else
                        target = serv.locations[i].index;
                }
                else
                {
                    target = serv.locations[i]._return;
                    status = "301";
                }
            }
        }
    }
}

void Request::long_uri(Server &serv)
{
    std::cout << "" <<std::endl;
    std::vector<std::string> uri;
    ft_split(target, "/", uri);
    target = "";
    int flag2;

    for (int j = 0; j < uri.size(); j++)
    {
        flag2 = 0;
        for (int i = 0; i < serv.locations.size(); i++)
        {
            if (uri[j] == serv.locations[i].NAME)
            {
                if ((method == "GET" && serv.locations[i].GET == false) 
                    || (method == "POST" && serv.locations[i].POST == false) 
                    || (method == "DELETE" && serv.locations[i].DELETE == false))
                        status = "405";
                else if (serv.locations[i]._return == "")
                {
                    if (serv.locations[i].index == "")
                    {
                        if (serv.locations[i].autoindex == true)
                            target += serv.locations[i].root;
                        else
                            status = "403";
                    }
                    else
                    {
                        target = serv.locations[i].index;
                        return;
                    }    
                }
                else
                {
                    target = serv.locations[i]._return;
                    status = "301";
                    return;
                }
                flag_uri = 1;
                flag2 = 1;
                if(status != "200")
                    return;
            }
        }
        if (!flag2)
            target += uri[j];
        if (j < uri.size() - 1)
        {
            if (target[target.size() - 1] != '/')
                target += "/";
        }
    }
}

void Request::directory_moved_permanently()
{
    struct stat fileStat;
  
    if (access(target.c_str(), F_OK) == 0)
    {
        if (stat(target.c_str(), &fileStat) == 0)
        {
            if (S_ISDIR(fileStat.st_mode))
            {
                if (target[target.length() - 1] != '/')
                {
                    status = "301"; 
                    target += "/";
                }
            }
        }
    }
}

void Request::encoded_uri()
{
    std::string newUri;
    std::map<std::string, char> myMap;

    myMap.insert(std::pair<std::string, char>("20", ' '));
    myMap.insert(std::pair<std::string, char>("3C", '<'));
    myMap.insert(std::pair<std::string, char>("3E", '>'));
    myMap.insert(std::pair<std::string, char>("23", '#'));
    myMap.insert(std::pair<std::string, char>("25", '%'));
    myMap.insert(std::pair<std::string, char>("3F", '?'));
    myMap.insert(std::pair<std::string, char>("26", '&'));
    myMap.insert(std::pair<std::string, char>("2F", '/'));
    myMap.insert(std::pair<std::string, char>("5C", '\\'));
    myMap.insert(std::pair<std::string, char>("3A", ':'));
    myMap.insert(std::pair<std::string, char>("3B", ';'));
    myMap.insert(std::pair<std::string, char>("3D", '='));
    myMap.insert(std::pair<std::string, char>("2B", '+'));
   
    for (int i = 0; i < target.length(); i++)
    {
        if (target[i] == '%')
        {
            std::string value(target.substr(i + 1, 2));
            newUri += myMap[value];
            i += 2;
        }   
        else
            newUri += target[i];
    }
    target = newUri;
}