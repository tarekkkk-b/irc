# include <iostream>
# include <sstream>
# include <stdexcept>

# include "inc/Server.hpp"

int strToIntSafe (std::string intStr)
{
    int num;
    std::string remaining;
    std::stringstream intParse(intStr);

    if (!(intParse >> num) || (intParse >> remaining))
        throw std::invalid_argument("invalid Port Number");
    
    return num;
}
    
int main(int argc, char **argv)
{
    try
    {
        if(argc != 3)
            throw std::invalid_argument("Program arguments should be <port> <password>");
        int portInt = strToIntSafe(argv[1]);
        Server server(portInt, argv[2]);
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
}