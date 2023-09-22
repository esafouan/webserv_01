#include "server.hpp"

location::location()
{
    NAME = "";
    root = "";    
    index = "";
    _return = "";  
}

int location::post(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.POST = true;
    else if (hold[1] == "off")
        location.POST = false;
    else
        return 0;
    return 1;
}

int location::get(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.GET = true;
    else if (hold[1] == "off")
        location.GET = false;
    else
        return 0;
    return 1;
}

int location::auto_index(location &location, std::vector<std::string> &hold)
{
     if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.autoindex = true;
    else if (hold[1] == "off")
        location.autoindex = false;
    else
        return 0;
    return 1;
}

int location::deletee(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.DELETE = true;
    else if (hold[1] == "off")
        location.DELETE = false;
    else
        return 0;
    return 1;
}

int location::name(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if(hold[1] != "/" && hold[1][0] == '/')
        throw error_config();
    location.NAME = hold[1];
    return 1;
}

int location::rreturn(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location._return = hold[1];
    return 1;
}

int location::root_name(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location.root = hold[1];
    return 1;
}

int location::Index(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location.index = hold[1];
    return 1;
} 

location::~location()
{}