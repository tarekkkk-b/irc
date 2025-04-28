#ifndef SERVER_HPP
# define SERVER_HPP
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <iostream>
 #include <arpa/inet.h>

class Server
{
    private:
        int _servPort;
        std::string _servPass;
        int _servFd;
        
        Server();

    public:
        Server(int port, std::string password);
        Server(Server const & other);
        Server & operator = (Server const & rhs);
        ~Server();

        void initSocket();

};

#endif