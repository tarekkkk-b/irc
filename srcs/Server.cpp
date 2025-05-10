#include "../inc/Server.hpp"

Server::Server() {}

Server::Server(int port, std::string password)
{
    this->_servPort = port;
    this->_servPass = password;
    this->clientcount = 0;

    std::cout << "Server initialized at port: " << this->_servPort;
    std::cout << " | password: " << this->_servPass << std::endl;
    this->initServerSocket();

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

void Server:: initServerSocket()
{
    this->_servFd= socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->_servPort);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(this->_servFd,(struct sockaddr*)&server_addr,sizeof(server_addr))==0)
        listen(this->_servFd,5);
    this-> kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }
    handleEvents();
}

void Server:: registerEvents(int fd , int16_t  filter)
{
    struct kevent ev;
    EV_SET(&ev, fd, filter, EV_ADD, 0, 0, NULL);
    if (kevent(this->kq, &ev, 1, NULL, 0, NULL) == -1)
    {
        perror("kevent register fd");
        exit(EXIT_FAILURE);
    }
}

std::string readLine(int fd) 
{
    std::string line;
    char ch;
    ssize_t n;
    while ((n = read(fd, &ch, 1)) == 1)
    {
        if (ch == '\n') 
            break;        
        line += ch;                    
    }
    return line;
}

 void Server :: registerChannelCients(std::vector <int> channelClients)
 {
    for( unsigned long i = 0; i < channelClients.size();i++)
    {
        registerEvents( channelClients[i], EVFILT_WRITE);
    }
    
 }


void Server :: handleEvents()
{
    std::string message;
    std::cout<<"im here in handle events"<<"\n";
    registerEvents(this -> _servFd,EVFILT_READ);
    struct kevent ev[MAX_EVENTS];
    while (true)
    {
       int  en =  kevent(this->kq, NULL, 0, ev,MAX_EVENTS , NULL) ;
       for(int  i = 0 ;i < en ;i++)
       {
            struct kevent event = ev[i];
            if(event.ident == this ->_servFd)
            {
                int client_fd = accept(this ->_servFd,NULL,NULL);
                std::cout<<"one client has been connected "<<"this is is his fd"<<client_fd<<"\n";
                registerEvents(client_fd,EVFILT_READ);
            }
            else if (event.filter == EVFILT_READ)
            {
                message = readLine(event.ident);
                std::cout<<message<<std::endl; 
                // this will be passes to maha in which she will parse it  and do the command  and the continue if it is not a message
                // if it is a message i will registerChannelCients(std::vector <int> channelClients)
                // and then the cliet will be an event with writing filter and it will go for the next else 
                    //   know the clients in the channel and then loop through them ad register them as clients for writing  
            }
            else
            {
                write(1, message.c_str(), message.size());
            }
       }

    }

}





// void Server:: registerEvents(int fd)
// {
    

//     // struct kevent {
//     //     uintptr_t  ident;     // Identifier (e.g. FD you're watching)
//     //     int16_t    filter;    // What kind of event (e.g. EVFILT_READ)
//     //     uint16_t   flags;     // What to do with it (EV_ADD, EV_DELETE, etc.)
//     //     uint32_t   fflags;    // Extra filter-specific options (rarely used for sockets)
//     //     intptr_t   data;      // Info about the event (e.g. bytes ready to read)
//     //     void      *udata;     // Your custom data (context pointer, optional)
//     // };
//     struct kevent ev;
//     // tell the kernal what to watch is like im telling the kernal please watch for me any read event on the srever socket
//     EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
//     // the events your getting please register for the in the kq
//     // int kevent(int kq, 
//     //    const struct kevent *changelist, int nchanges, 
//     //     struct kevent *eventlist, int nevents, 
//     //     const struct timespec *timeout);
//     if (kevent(this->kq, &ev, 1, NULL, 0, NULL) == -1)
//     {
//         perror("kevent register listen_fd");
//         exit(EXIT_FAILURE);
//     }
    
// }
  // for std::cout