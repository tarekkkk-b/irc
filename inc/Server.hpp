#ifndef SERVER_HPP
# define SERVER_HPP
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <iostream>
 #include <arpa/inet.h>
 #include <vector>
 #include "Client.hpp"

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        int _servFd;
        std:: vector <Client> clients_list;
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();

        void initSocket();
        void handle_connections();

};

#endif