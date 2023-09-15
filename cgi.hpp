#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
class CGI
{
	private :
		std::string _filename;
		char **arr;
		std::string cgiresp;
	public :
		CGI(const std::string &filename);
		~CGI();
		int check_ext();
		int execute(std::string body);

};

#endif