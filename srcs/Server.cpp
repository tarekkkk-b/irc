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
    server_addr.sin_port = htons(this->_servPort);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(this->_servFd,(struct sockaddr*)&server_addr,sizeof(server_addr))==0)
        listen(this->_servFd,5);
    // std::cout<<"hi this is my fd"<<this->_servFd<<"\n";
}

void Server:: handle_connections()
{
    int kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }

    // struct kevent {
    //     uintptr_t  ident;     // Identifier (e.g. FD you're watching)
    //     int16_t    filter;    // What kind of event (e.g. EVFILT_READ)
    //     uint16_t   flags;     // What to do with it (EV_ADD, EV_DELETE, etc.)
    //     uint32_t   fflags;    // Extra filter-specific options (rarely used for sockets)
    //     intptr_t   data;      // Info about the event (e.g. bytes ready to read)
    //     void      *udata;     // Your custom data (context pointer, optional)
    // };
    
    struct kevent ev;
    // tell the kernal what to watch is like im telling the kernal please watch for me any read event on the srever socket
    EV_SET(&ev, this->_servFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    // the events your getting please register for the in the kq
    // int kevent(int kq, 
    //    const struct kevent *changelist, int nchanges, 
    //     struct kevent *eventlist, int nevents, 
    //     const struct timespec *timeout);
    if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
    {
        perror("kevent register listen_fd");
        exit(EXIT_FAILURE);
    }

}