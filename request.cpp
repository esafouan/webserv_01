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

void pars_headers(std::vector<std::pair<std::string, std::string> > &headers, std::string &stat, std::string method)
{

}

std::string replace_slash_in_target(Server &serv, std::string &targ, int *flag)
{
    std::string state = "200";

    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME == "/")
        {
            std::cout << "localisation = "<< serv.locations[i]._return << std::endl;
            if (serv.locations[i]._return == "")
            {
                if(serv.locations[i].index == "")
                {
                    if (serv.locations[i].autoindex == true)
                        targ = serv.locations[i].root;
                    else
                        state = "403";
                }
                else
                    targ = serv.locations[i].root + "/"+ serv.locations[i].index; 
            }   
            else
            {
                std::cout << "heeeere "<<std::endl;
                targ = serv.locations[i]._return;
                state = "301";
            }            
            *flag = 1;
        }
    }
    return state;
}

int count_slash(std::string tar)
{
    int count = 0;
    for (int i = 0; i < tar.length(); i++)
        if (tar[i] == '/')
            count++;
    return (count);
}

void short_uri(std::string &tar, Server &serv, int *flag_uri)
{
    for (int i = 0; i < serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME != "/")
        {
            if (tar == serv.locations[i].NAME)
            {
                if(serv.locations[i]._return == "")
                    tar = serv.locations[i].root;
                else
                {
                    tar = serv.locations[i]._return;
                    //status must be 301
                }
                *flag_uri = 1;
            }
        }
    }
}

void long_uri(std::string &tar, Server &serv, int *flag_uri)
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
                if(serv.locations[i]._return == "")
                    tar += serv.locations[i].root;
                else
                {
                    tar += serv.locations[i]._return;
                    //status must be 301
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
        status = "405";
    else if (method != "GET" && method != "DELETE" && method != "POST")
    {
        if (method == "PUT" || method == "HEAD" || method == "TRACE" || method == "CONNECT")
            status = "501";
        else 
            status = "404";
    }
    else if (target.size() > 2048)
        status = "414";
    else
    {
        int flag = 0;

        if (target == "/")
            status = replace_slash_in_target(serv, target, &flag);
        else if (target[0] == '/')
            target = target.substr(1);

        if (count_slash(target) == 0 && target != "/")
            short_uri(target, serv, &flag_uri);
        else if (count_slash(target) >= 1)
            long_uri(target, serv, &flag_uri);
    
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
        status = "400";
    else if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) != "chunked")

        status = "501";
    else if (!find_key("Transfer-Encoding", StoreHeaders) && !find_key("Content-Length", StoreHeaders) && method == "POST")
        status = "400";
    else if (find_key("Transfer-Encoding", StoreHeaders) && find_key("Content-Length", StoreHeaders) && method == "POST")
        status = "400";
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
 
    this->open_boundry_file = req.open_boundry_file;
    this->rest_of_boundry = req.rest_of_boundry;
    
    this->chunk_size =  req.chunk_size;
    this->calcul_chunk_flag = req.calcul_chunk_flag;
    this->Bytes_readed = req.Bytes_readed;
    this->flag_uri = req.flag_uri;
    this->rest_of_buffer = req.rest_of_buffer;
    this->uri_for_response = req.uri_for_response;
    this->boundary_separater = req.boundary_separater;
    this->time = req.time;

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
    {
        Post_status = "Chunked/boundary";
        outfile_name = get_current_time() + ".txt";
        outfile.open(outfile_name.c_str(), std::ios::binary);
        boundary_separater = get_separater(valueOfkey("Content-Type", StoreHeaders));
    }
       
    else if (find_key("Transfer-Encoding", StoreHeaders) && valueOfkey("Transfer-Encoding", StoreHeaders) == "chunked")
    {
        outfile_name = "directorie/upload/" + get_current_time() + extension;
        outfile.open(outfile_name.c_str(), std::ios::binary);
        Post_status = "chunked";
    }
        
    else if (find_key("Content-Type", StoreHeaders) && valueOfkey("Content-Type", StoreHeaders).find("boundary") != std::string::npos)
    {
        Post_status = "boundary";
        boundary_separater = get_separater(valueOfkey("Content-Type", StoreHeaders));
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

// void    get_paths_to_delete(std::string path, std::vector<std::string> &paths)
// {
//     DIR *my_dir = opendir(path.c_str());
    
//     if(my_dir)
//     {
//         struct dirent* content;
//         while((content = readdir(my_dir)) != NULL)
//             std::cout << "Name = "<< content->d_name << " Type = " << (int)content->d_type  << std::endl;
//         exit(0);
//     }
// }

int DeleteDirectoryRecursive(const char* path)
{
    DIR* dir = opendir(path);

    if (!dir) {
        perror("Error opening directory");
        return 0;
    }

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char entryPath[1024];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);

            struct stat statbuf;
            if (lstat(entryPath, &statbuf) == 0) {
                if (S_ISDIR(statbuf.st_mode)) {
                    DeleteDirectoryRecursive(entryPath);
                } else {
                    if (remove(entryPath) != 0) {
                        return 0;
                    }
                }
            } else {
                return 0;
            }
        }
    }
    closedir(dir);
    if (rmdir(path) != 0) {
       return 0;
    }
    return 1;
}

void Request::Delete_methode()
{
    struct stat fileStat;
    std::vector<std::string> paths_to_delete;
    
    if (access(target.c_str(), F_OK) == 0)
    {
        if (stat(target.c_str(), &fileStat) == 0)
        {
            if (S_ISDIR(fileStat.st_mode))
            {
               if(DeleteDirectoryRecursive(target.c_str()))
                    status = "200";
                //get_paths_to_delete(target, paths_to_delete);
                //if(rmdir(target.c_str()) == 0)
                //    
                else
                   status = "404";
            }
            else if(!S_ISDIR(fileStat.st_mode))
            {
                if (fileStat.st_mode & S_IWUSR)
                {
                     std::remove(target.c_str());
                      status = "200";
                }
                   
                else
                    status = "404"; 
            }
            else
                status = "404";
        }
   }
   else
        status = "404";
}

void Request::get_target_page()
{
    if (status == "400")
        target = "error/400.html";
    else if (status == "401")
        target = "error/401.html";
    else if(status == "403")
        target = "error/403.html";
    else if(status == "404")
        target = "error/404.html";
    else if(status == "500")
        target = "error/500.html";
    else if(status == "501")
        target = "error/501.html";
    else if(status == "503")
        target = "error/503.html";
}


Request::Request(std::string req, Server server)
{
    int filmap = 0;
    std::vector<std::string> myreq;
    std::vector<std::string> myHeaders;
    this->status = "200";
    this->header_flag = 0;
    this->fd_file = -1;
    this->endOfrequest = 1;
    this->lenght_Readed = 0;
    calcul_chunk_flag = 0;
    Bytes_readed = 1024;
    this->open_boundry_file= 0;
    this->chunk_size = 0;
    this->rest_of_buffer = "";
    rest_of_boundry = "";
    flag_uri = 0;
    time = "";
   
    ft_split(req, "\r\n", myHeaders);
    fill_type(method, target, httpVersion, myHeaders, &filmap);    
    
    if (filmap)
    {
        status = "400";
        error("Bad Request");
    }

    uri_for_response = target;

    fill_headers(StoreHeaders, myHeaders);
    
    // print Headers
    //for (int i = 0; i < StoreHeaders.size(); i++)
    //  std::cout << "val = " << StoreHeaders[i].first << " key = " << StoreHeaders[i].second << std::endl;

    Request::error_handling(server);
    this->lenght_of_content = std::atoi(valueOfkey("Content-Length", StoreHeaders).c_str());
    extension = generate_extention(valueOfkey("Content-Type", StoreHeaders));

    if (method == "POST" && status == "200")
    {
        this->endOfrequest = 0;
        Request::get_post_status();
    }
    if (status != "200")
        this->endOfrequest = 1;
    
    if (status == "200" && access(target.c_str(), F_OK) == -1)
        status = "404";
    
    if (method == "DELETE" && status == "200")
    {
        Request::Delete_methode();
        if (status == "200")
            target = "error/200.html";
    }
      
    if(status == "200")
    {
        struct stat fileStat;

        if (stat(target.c_str(), &fileStat) == 0)
        {
            if (S_ISDIR(fileStat.st_mode))
            {
                int pos = target.length();
                if(target[pos - 1] != '/')
                {
                    target.append("/");
                    status = "301";
                }
            }
        }
    }
    Request::get_target_page();
}

Request::~Request()
{
}