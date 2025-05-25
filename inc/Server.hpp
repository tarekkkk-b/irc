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
#include <cstdlib>

#define MAX_EVENTS 1024

class Channel;

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        uintptr_t _servFd;
        std::map <int, Client * > clients_list;
        std:: map <std::string, Channel * > _channels;
        int kq;
        std::vector <int> clientsSocket;
        int clientcount;

        message parseChannelCommand(std::string message, Client const & sender);
        message handleJoin(std::vector<std::string> command, Client const & sender);
        message handlePrivMsg(std::string message, std::vector<std::string> command, Client const & sender);
        message handleInvite(std::vector<std::string> command, Client const & sender);
        message handleTopic(std::vector<std::string> command, Client const & sender);
        message handleKick(std::vector<std::string> command, Client const & sender);
        message handleMode(std::vector<std::string> command, Client const & sender);
        message parseClientCommand(std::vector<std::string> msg, Client &sender);
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();

        void initServerSocket();
        void registerEvents(int fd , int16_t  filter);
        void handleEvents();
        void registerChannelCients(std::vector <Client * const> channelClients);

        std::string getServPass() const;

        void    authClient(Client &sender);

        message determinCommandSide(const std::string msg, Client &sender);

        Channel * getChannel (std::string name);
        Client * getClientByFd (int socketFd);
        Client * getClientByNick (std::string name);


};

#endif