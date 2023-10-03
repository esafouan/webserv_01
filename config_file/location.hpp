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
        std::string upload_path;
        bool POST;
        bool autoindex;
        bool GET;
        bool DELETE;
        bool cgi;
        bool upload_s;
        std::vector <std::string> duplicate_in_location;

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
        int cgi_state(location &location, std::vector<std::string> &hold);
        int upload_state(location &location, std::vector<std::string> &hold);
        int uploadpath(location &location, std::vector<std::string> &hold);
        int loc_duplicate();
        
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
            std::cout << "cgi :\n" << cgi << std::endl;
            std::cout << "upload :\n" << upload_s << std::endl;
        }
        ~location();
};
