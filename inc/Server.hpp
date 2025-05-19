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
# include "types.hpp"

#define MAX_EVENTS 1024

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        uintptr_t _servFd;
        std::map <std::string, Client * > clients_list;
        std:: map <std::string, Channel * > _channels;
        int kq;
        std::vector <int> clientsSocket;
        int clientcount;
        message parseChannelCommand(std::vector<std::string> command, Client const & sender);
        message parseClientCommand(std::vector<std::string> command, Client const & sender);
        
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

        message    Server::determinCommandSide(const std::string msg, Client const &sender)

        Channel * getChannel (std::string name);


};

#endif