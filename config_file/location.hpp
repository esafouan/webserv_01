#pragma once

#include <ctime>
#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <dirent.h>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <unistd.h>
#include <cerrno>
#include <sys/stat.h>

class location
{
    public:
        std::string NAME ;
        std::string  root ;    
        std::string index ;
        std::string _return ;  
        bool POST;
        bool autoindex;
        bool GET;
        bool DELETE;

    public :
        location();
        int post(location &location, std::vector<std::string> &hold);
        int get(location &location, std::vector<std::string> &hold);
        int auto_index(location &location, std::vector<std::string> &hold);
        int deletee(location &location, std::vector<std::string> &hold);
        int name(location &location, std::vector<std::string> &hold);
        int rreturn(location &location, std::vector<std::string> &hold);
        int root_name(location &location, std::vector<std::string> &hold);
        int Index(location &location, std::vector<std::string> &hold);
        void    print()
        {
            std::cout << NAME << std::endl;
            std::cout << "root :\n" << root << std::endl;
            std::cout << "autoindex :\n" << autoindex << std::endl;
            std::cout << "POST :\n" << POST << std::endl;
            std::cout << "GET :\n" << GET << std::endl;
            std::cout << "DELETE :\n" << DELETE << std::endl;
            std::cout << "index :\n" << index << std::endl;
            std::cout << "return :\n" << _return << std::endl;
        }
        ~location();
};
