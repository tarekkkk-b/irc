# include <iostream>
# include <sstream>
# include <stdexcept>

# include "Server.hpp"

int strToIntSafe (std::string intStr)
{
    int num;
    std::string remaining;
    std::stringstream intParse(intStr);

    if (!(intParse >> num) || (intParse >> remaining) || num < 6660 || num > 6669)
        throw std::invalid_argument("invalid Port Number");
    
    return num;
}

void signalHandler(int signal)
{
    if(signal == 2)
    {
        if(SERVFD!= -1)
        {
            close (SERVFD);
        }
        exit(0);
    }
}
#include <signal.h>
    
int main(int argc, char **argv)
{
    signal (SIGINT,signalHandler);
    try
    {
        if(argc != 3)
            throw std::invalid_argument("Program arguments should be <port> <password>");
        int portInt = strToIntSafe(argv[1]);
        Server server(portInt, argv[2]);
     
        // client connect 
        // socketfd created for him
        // authenticate client 
        // client join channel
        // client send and recive messages
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
}