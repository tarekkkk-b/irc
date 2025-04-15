#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();
};

#endif