#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/event.h>
# include <iostream>
# include <arpa/inet.h>
# include <vector>
# include "Client.hpp"
# include <unistd.h>
# include <unistd.h> 
# include <string>     
# include <iostream>
# include <utility>
# include "Channel.hpp"
# include <map>

#define MAX_EVENTS 1024

class Server
{
    private:
        typedef std::pair<std::string, std::vector <Client const *> > message;
        int _servPort;
        std::string _servPass;
        uintptr_t _servFd;
        std::map <std::string, Client const * > clients_list;
        std:: map <std::string, Channel const * > _channels;
        int kq;
        std::vector <int> clientsSocket;
        int clientcount;
        message parseChannelCommand(std::vector<std::string> command, Client const & sender);
        
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