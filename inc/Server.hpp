#ifndef SERVER_HPP
# define SERVER_HPP
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <sys/event.h>
 #include <iostream>
 #include <arpa/inet.h>
 #include <vector>
 #include "Client.hpp"
 #define MAX_EVENTS 100

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        int _servFd;
        std:: vector <Client> clients_list;
        int kq;
        std::vector <int> clientsSocket;
        int clientcount;
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();

        void initServerSocke();
        void registerListenFd();
        void handleEvents();

};

#endif