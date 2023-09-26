#include "Request.hpp"

std::string Request::replace_slash_in_target(Server &serv, std::string &targ, int *flag)
{
    std::string state = "200";
    int no_root_location = 0;
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME == "/")
        {
            no_root_location = 1;
            if (serv.locations[i]._return == "")
            {
                if (serv.locations[i].index == "")
                {
                    if (serv.locations[i].autoindex == true)
                        targ = serv.locations[i].root;
                    else
                        state = "403";
                }
                else
                    targ = serv.locations[i].root + "/" + serv.locations[i].index;
            }
            else
            {
                targ = serv.locations[i]._return;
                state = "301";
            }
            *flag = 1;
        }
    }

    if (!no_root_location)
    {
        targ = serv.root;
    }
        
    return state;
}

int Request::count_slash(std::string tar)
{
    int count = 0;
    for (int i = 0; i < tar.length(); i++)
        if (tar[i] == '/')
            count++;
    return (count);
}

void Request::short_uri(std::string &tar, Server &serv, int *flag_uri)
{
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME != "/")
        {
            if (tar == serv.locations[i].NAME)
            {
                if (serv.locations[i]._return == "")
                    tar = serv.locations[i].root;
                else
                {
                    tar = serv.locations[i]._return;
                    // status must be 301
                }
                *flag_uri = 1;
            }
        }
    }
}

void Request::long_uri(std::string &tar, Server &serv, int *flag_uri)
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
                if (serv.locations[i]._return == "")
                    tar += serv.locations[i].root;
                else
                {
                    tar += serv.locations[i]._return;
                    // status must be 301
                }
                *flag_uri = 1;
                flag2 = 1;
            }
        }
        if (!flag2)
            tar += uri[j];
        if (j < uri.size() - 1)
            tar += "/";
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
