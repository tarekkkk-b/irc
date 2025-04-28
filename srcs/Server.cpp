#include "../inc/Server.hpp"

Server::Server() {}

Server::Server(int port, std::string password)
{
    this->_servPort = port;
    this->_servPass = password;

    std::cout << "Server initialized at port: " << this->_servPort;
    std::cout << " | password: " << this->_servPass << std::endl;
    this->initSocket();

    std::cout << "\n╔════════════════════════════╗\n";
    std::cout << "║ IRC Server is listening... ║\n";
    std::cout << "║ Port: " << _servPort << "                 ║\n";
    std::cout << "╚════════════════════════════╝\n\n";
}

Server::Server(Server const & other)
{
    *this = other;
}

Server & Server::operator = (Server const & rhs)
{
    this->_servPort = rhs._servPort;
    this->_servPass = rhs._servPass;

    return (* this);
}

Server::~Server()
{
    std::cout << "╔═════════════════════════════════╗\n";
    std::cout << "║  IRC Server is shutting down... ║\n";
    std::cout << "╚═════════════════════════════════╝\n";
}

void Server:: initSocket()
{
    this->_servFd= socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(bind(this->_servFd,(struct sockaddr*)&server_addr,sizeof(server_addr))==0)
        listen(this->_servFd,5);
    // std::cout<<"hi this is my fd"<<this->_servFd<<"\n";
}