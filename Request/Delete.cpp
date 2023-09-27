#include "Request.hpp"

void Request::Delete(const char *path)
{
    DIR *dir = opendir(path);
   
    if (status == "200")
    {
        struct dirent *entry;
        while ((entry = readdir(dir)))
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                std::string entryPath(path); 
    
                entryPath += "/";
                entryPath += entry->d_name;
                struct stat statbuf;
                if (lstat(entryPath.c_str(), &statbuf) == 0)
                {
                    if (S_ISDIR(statbuf.st_mode))
                        Delete(entryPath.c_str());
                    else
                    {
                        if (access(entryPath.c_str(), W_OK) == 0)
                            std::remove(entryPath.c_str());
                        else
                            status = "403"; 
                    }
                }                    
            }
        }
        closedir(dir);
        if (access(path, W_OK) == 0)
            rmdir(path);    
        else
            status = "403";
    }
}

void Request::Delete_methode()
{
    struct stat fileStat;

    if (stat(target.c_str(), &fileStat) == 0)
    {
        if (S_ISDIR(fileStat.st_mode))
            Delete(target.c_str());
        else
        {
            if (access(target.c_str(), W_OK) == 0)
                std::remove(target.c_str());
            else
                status = "403";
        }
    }
}
