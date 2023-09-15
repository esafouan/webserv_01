#include "cgi.hpp"

void handleSignalTimeout(int sigo)
{
	if (sigo == SIGALRM) 
		exit(1);
}

std::string getLastSubstring(const std::string& input, char delimiter) 
{
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = input.find(delimiter, start)) != std::string::npos) 
	{
        tokens.push_back(input.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(input.substr(start));
    if (!tokens.empty())
        return tokens.back();
	else
        return "";
}

CGI::CGI(const std::string &filename)
{
    this->_filename = filename;
    this->arr = new char*[3];
    for (int i = 0; i < 3; ++i)
        arr[i] = NULL;
}

int CGI::check_ext()
{
    std::string tempo = getLastSubstring(this->_filename,'.');
    for(unsigned long i = 0; i < tempo.length(); ++i)
        tempo[i] = std::tolower(tempo[i]);
    if(tempo == "py")
        return(1);
    else if(tempo == "php")
        return(2);
    else
        return(0);

}

int CGI::execute(std::string body)
{
    int check = this->check_ext();
    if(!check)
        return(1);
    else
    {
        if(check == 1)
            std::string exec = "/usr/bin/python3";
        else if(check == 2)
            std::string exec = "/usr/bin/php";
        char *inf = getenv("PATH_INFO");
        std::string infstr(inf);
        std::string path = infstr + this->_filename;
        std::strcpy(this->arr[0],infstr.c_str());
        std::strcpy(this->arr[1],path.c_str());
    }
    extern char **environ;
    check = 0;
    int fd[2];
    int tmp = dup(0);
    if (pipe(fd) < 0)
        return (-1);
    pid_t pid = fork();
    if (pid < 0)
        return (-1);
    if (pid == 0)
    {
        dup2(fd[1], 1);
		close(fd[1]);

        std::FILE* file;
        const char* name = "/tmp/.body";
        file = std::fopen(name, "w+"); 
        if (file) 
        {
            for (size_t i = 0; i < body.size() ; ++i) {
                std::fputc(body[i], file);
            }
            std::rewind(file);
        }
        dup2(fileno(file), 0);
        std::fclose(file);
        char *str = getenv("SCRIPT_FILENAME");
        std::string filename(str);        
        if (filename != "/upload.py")        
            signal(SIGALRM, handleSignalTimeout);
        alarm(5); 
        if (execve(this->arr[0], this->arr, environ) < 0)
        {
            std::cerr << "execve failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    waitpid(pid, &check, 0);
    if (WIFSIGNALED(check) || check != 0)
        return (-1);
    dup2(fd[0], 0);
	close(fd[0]);
	close(fd[1]);
	char buffer[1];
	while (read(0, buffer, 1) > 0)
        this->cgiresp += buffer[0];
	dup2(tmp, 0);
	close(tmp);
    return (1);
}

CGI::~CGI()
{
    for (int i = 0; i < 3; ++i)
        delete[] arr[i];
    delete[] arr;
}
