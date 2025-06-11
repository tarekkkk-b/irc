#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <netinet/in.h>
// # include <sys/event.h>
# include <../libkqueue/include/sys/event.h>
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
#include <csignal>
#include <cstdio>
#include <algorithm>
#include <fcntl.h>   
#include <cstring> 



#define MAX_EVENTS 1024
extern int SERVFD;
extern int KQ;


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

        std::vector <Client * > parseChannelCommand(std::string message, Client  & sender);
        std::vector <Client * > handleJoin(std::vector<std::string> command, Client  & sender);
        std::vector <Client * > handlePrivMsg(std::string message, std::vector<std::string> command, Client  & sender);
        std::vector <Client * > handleInvite(std::vector<std::string> command, Client  & sender);
        std::vector <Client * > handleTopic(std::vector<std::string> command, Client  & sender);
        std::vector <Client * > handleKick(std::vector<std::string> command, Client  & sender);
        std::vector <Client * > handleMode(std::vector<std::string> command, Client  & sender);
        std::vector <Client * > parseClientCommand(std::vector<std::string> msg, Client &sender);
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();

        void initServerSocket();
        uintptr_t getServFd();
        void registerEvents(int fd , int16_t  filter);
        void handleEvents();
        void registerChannelCients(std::vector <Client * > channelClients);
       void deregisterEvent(int fd, int filterType);
        bool channelNameIsValid(const std::string &name);
        void cleanupAfterClient(Client *client, int fd);
        void sendMessage(int fd);
        void handleRecivers(std::string text,int fd);


        std::string getServPass() const;

        std::vector<Client * > determinCommandSide(const std::string msg, Client &sender);

        Channel * getChannel (std::string name);
        Client * getClientByFd (int socketFd);
        Client * getClientByNick (std::string name);

        std::vector <Client * >    setClientsBuffer(std::vector<Client * > clients, std::string message);
        void removeClientFromChannels(Client *client);


};

#endif