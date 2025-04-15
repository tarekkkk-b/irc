#include "Server.hpp"

Server::Server() {}

Server::Server(int port, std::string password)
{
    this->_servPort = port;
    this->_servPass = password;

    std::cout << "Server initialized at port: " << this->_servPort;
    std::cout << " | password: " << this->_servPass << std::endl;

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