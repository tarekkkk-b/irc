#include "../inc/Server.hpp"
#include "Server.hpp"

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

std::string Server::getServPass() const
{
    return this->_servPass;
}

void Server::authClient(Client &sender)
{
    if (!sender.getNick().empty() && !sender.getUser().empty() && sender.getPass())
        sender.setAuth(true);
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
                write(event.ident, message.c_str(), message.size());
            }
       }

    }

}

bool isExistingChannel (std::string const & name)
{
    // std::vector<Channel *>::iterator = _channels.begin();
    // for ()
    return 1;
}

message Server::parseChannelCommand(std::vector<std::string> command, Client const & sender)
{
    std::string commands[] = { "JOIN", "PRIVMSG", "INVITE", "TOPIC", "KICK", "MODE" };
    // check command syntax is valid
    // check command syntax
    // check if channel exists
    // extract channel object
    // pick the right command to perform from the channel commands

}

static std::vector<std::string> splitWords(const std::string msg)
{
	std::istringstream iss(msg);
	std::vector<std::string> words;
	std::string temp;

	while (iss >> temp)
		words.push_back(temp);
	return words;
}

message    Server::determinCommandSide(const std::string msg, Client &sender)
{
    std::vector<std::string> words = splitWords(msg);
    std::string error_msg = "421: " + sender.getName() + " " + words[0] + " " + ": Unknown Command.";
    message error = std::make_pair("", std::vector<const Client *>(1, &sender));
    std::string commands[] = { "JOIN", "PRIVMSG", "INVITE", "TOPIC", "KICK", "MODE", "NAME", "USER", "PASS", "INVALID" };
    int i = 0;
    while (i < 10)
    {
        if (commands[i] == words[0])
            break ;
        i++;
    }
    if (i = 9)
        sender.setBuffer(error_msg);
    return ((i >= 0 && i <= 5) ? this->parseChannelCommand(words, sender) : (i >= 6 && i <= 8) ? this->parseClientCommand(words, sender) : error);
}

message Server::parseClientCommand(std::vector<std::string> msg, Client &sender)
{
    std::string commands[] = { "NICK", "USER", "PASS" };
    int i = 0;
    while (i < 3)
    {
        if (commands[i] == msg[0])
            break ;
        i++;
    }
    switch (i)
    {
    case 0:
        return (sender.setNick(msg, *this));
    case 1:
        return (sender.setUser(msg, *this));
    case 2:
        return (sender.setPass(msg, *this));
    // case 3:
    //     return (sender.setName(msg));
    }
	return message();
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