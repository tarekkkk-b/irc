# include "Server.hpp"

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
	for (std::map<int, Client*>::iterator it = clients_list.begin(); it != clients_list.end(); ++it)
        delete it->second;
    clients_list.clear();

    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;

    _channels.clear();
	std::cout << "╔═════════════════════════════════╗\n";
	std::cout << "║  IRC Server is shutting down... ║\n";
	std::cout << "╚═════════════════════════════════╝\n";
}

uintptr_t Server:: getServFd()
{
	return(this->_servFd);
}

void Server:: initServerSocket()
{
	this->_servFd= socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(this->_servPort);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	// is used to allow the socket to reuse a local address (i.e., port) that's in the TIME_WAIT state.
	int yes = 1;
	if (setsockopt(this->_servFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
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
		{
			line += ch;  
				break; }       
		line += ch;                    
	}
	return line;
}

 void Server :: registerChannelCients(std::vector <Client * > channelClients)
 {
	for( unsigned long i = 0; i < channelClients.size();i++)
	{
		if (channelClients[i]->getSocketFd()!= -1)
		   registerEvents( channelClients[i]->getSocketFd(), EVFILT_WRITE);
	}
	
 }

std::string Server::getServPass() const
{
    return this->_servPass;
}

void Server::authClient(Client &sender)
{
	std::cout << "---------Auth------------\n";
	std::cout << "client Nick: " << sender.getNick() << std::endl;
	std::cout << "client User: " << sender.getUser() << std::endl;
	std::cout << "client Name: " << sender.getName() << std::endl;
	std::cout << "client Pass: " << sender.getPass() << std::endl;
	std::cout << "-------------------------\n\n";
    if (!sender.getNick().empty() && !sender.getUser().empty() && sender.getPass())
	{
		sender.setAuth(true);
	}
}

const Client *getClientObject(int fd,std::vector<Client *> ClientsToSend)
{
	for(size_t i = 0; i < ClientsToSend.size();i++)
	{
		if(ClientsToSend[i]->getSocketFd() == fd)
			return(ClientsToSend[i]);
	}
	return(NULL);
}

void Server::deregisterEvent(int fd, int filterType) {
    struct kevent evSet;
    EV_SET(&evSet, fd, filterType, EV_DELETE, 0, 0, NULL);
    kevent(this->kq, &evSet, 1, NULL, 0, NULL);
}
// void removeClientSilently(Client * client);
// std:: map <std::string, Channel * > _channels
void Server:: removeClientFromChannels(Client *client)
{
	// for(size_t i =0 ; i < client->getChannels().size();i++)
	// {
	// 	std::cout << "im here in removeClientFromChannels \n";
	// 	_channels[client->getChannels()[i]]->removeClientSilently(client);
	// }
	client->destroyClient();

}

void Server :: handleEvents()
{
	std::string text;
	registerEvents(this -> _servFd,EVFILT_READ);
	struct kevent ev[MAX_EVENTS];
	std::vector <Client *> toSend;
	while (true)
	{
	   int  en =  kevent(this->kq, NULL, 0, ev,MAX_EVENTS , NULL) ;
	   for(int  i = 0 ;i < en ;i++)
	   {
			struct kevent event = ev[i];
			if(event.ident == this ->_servFd)
			{
				int client_fd = accept(this ->_servFd,NULL,NULL);
				std::cout<<"one client has been connected at fd -> "<<client_fd<<"\n";
				registerEvents(client_fd,EVFILT_READ);
			}
			else if (event.filter == EVFILT_READ)
			{
				Client *client = getClientByFd(event.ident);
				if (client == NULL)
				{
					// std::cout <<"im here the client is null\n";
						clients_list[event.ident] = new Client(event.ident);}
				text = readLine(event.ident);
				if (text.size()==0 && text.empty())
				{
					Client *client = getClientByFd(event.ident);
					removeClientFromChannels(client);
					close(event.ident);
					deregisterEvent(event.ident,EVFILT_READ);
					clients_list.erase(event.ident);
					delete client;
					continue;
				}
				else if(text.size()==1 && text[0] == '\n')
					continue;
				toSend = determinCommandSide(text, *getClientByFd(event.ident));
				// this->authClient(*client);
				std::cout<<"command: " << text;
				// std::cout<<"Number of clients to recieve response: " << toSend.size()<<"\n";
				registerChannelCients(toSend); 
			}
			else if (event.filter == EVFILT_WRITE)
			{
				Client *client = getClientByFd(event.ident);
				if (client && !client->getBuffer().empty())
				{
					std::cout << "buffer is:	" << client->getBuffer().c_str() << std::endl;
					ssize_t written = write(event.ident, client->getBuffer().c_str(), client->getBuffer().size());
					if (written > 0) {
						client->clearBuffer();
					}
					deregisterEvent(event.ident, EVFILT_WRITE); 
				}
			}
	   }

	}

}

Channel * Server::getChannel (std::string name)
{
	std::map <std::string, Channel *>::iterator channel = _channels.find(name);
	if (channel != _channels.end())
		return channel->second;
	return NULL;
}

Client * Server::getClientByNick (std::string name)
{
	for (std::map<int, Client*>::iterator it = clients_list.begin(); it != clients_list.end(); ++it) {
		if (it->second && it->second->getNick() == name)
			return it->second;
	}
	return NULL;
}

Client* Server::getClientByFd(int socketFd)
{
	std::map <int, Client *>::iterator client = clients_list.find(socketFd);
	if (client != clients_list.end())
		return client->second;
	return NULL;
}

std::vector <Client * > Server::setClientsBuffer(std::vector<Client * > clients, std::string message)
{
	for (std::vector<Client * >::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->setBuffer(message);
	}
	return clients;
}