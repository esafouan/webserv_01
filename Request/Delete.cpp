#include "Request.hpp"

int DeleteDirectoryRecursive(const char *path)
{
    DIR *dir = opendir(path);

    if (!dir)
    {
        perror("Error opening directory");
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char entryPath[1024];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);

            struct stat statbuf;
            if (lstat(entryPath, &statbuf) == 0)
            {
                if (S_ISDIR(statbuf.st_mode))
                {
                    DeleteDirectoryRecursive(entryPath);
                }
                else
                {
                    if (remove(entryPath) != 0)
                    {
                        return 0;
                    }
                }
            }
            else
            {
                return 0;
            }
        }
    }
    closedir(dir);
    if (rmdir(path) != 0)
    {
        return 0;
    }
    return 1;
}

void Request::Delete_methode()
{
    struct stat fileStat;
    //std::vector<std::string> paths_to_delete;

    if (access(target.c_str(), F_OK) == 0)
    {
        if (stat(target.c_str(), &fileStat) == 0)
        {
            if (S_ISDIR(fileStat.st_mode))
            {
                if (DeleteDirectoryRecursive(target.c_str()))
                    status = "200";
                // get_paths_to_delete(target, paths_to_delete);
                // if(rmdir(target.c_str()) == 0)
                //
                else
                    status = "404";
            }
            else if (!S_ISDIR(fileStat.st_mode))
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
