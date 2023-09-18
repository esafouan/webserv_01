#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    std::string input = "apple orange banana";
    std::istringstream iss(input);

    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    for (const std::string& t : tokens){
        std::cout << t << std::endl;
    }

    return 0;
}