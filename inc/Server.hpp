#ifndef SERVER_HPP
# define SERVER_HPP
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <sys/event.h>
 #include <iostream>
 #include <arpa/inet.h>
 #include <vector>
 #include "Client.hpp"
 #include <unistd.h>
 #include <unistd.h> 
#include <string>     
#include <iostream> 
 #define MAX_EVENTS 1024

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        uintptr_t _servFd;
        // std:: vector <Client> clients_list;
        int kq;
        std::vector <int> clientsSocket;
        int clientcount;
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();

        void initServerSocket();
        void registerEvents(int fd , int16_t  filter);
        void handleEvents();
       void registerChannelCients(std::vector <int> channelClients);

};

#endif