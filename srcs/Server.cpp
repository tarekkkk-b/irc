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
#include <sstream>
std::vector<std::string> splitString(const std::string& input) {
	std::vector<std::string> result;
	std::istringstream stream(input);
	std::string word;

	while (stream >> word) {
		result.push_back(word);
	}

	return result;
}

void Server:: initServerSocket()
{
	this->_servFd= socket(AF_INET,SOCK_STREAM,0);
	std::cout<<this->_servFd<<"\n";
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
			break;        
		line += ch;                    
	}
	return line;
}

 void Server :: registerChannelCients(std::vector <Client * > channelClients)
 {
	for( unsigned long i = 0; i < channelClients.size();i++)
	{
		registerEvents( channelClients[i]->getSocketFd(), EVFILT_WRITE);
	}
	
 }

std::string Server::getServPass() const
{
    return this->_servPass;
}

void Server::authClient(Client &sender)
{
	std::cout << "-------------------------\n";
	std::cout << sender.getNick() << std::endl;
	std::cout << sender.getUser() << std::endl;
	std::cout << sender.getPass() << std::endl;
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

void Server :: handleEvents()
{
	std::string text;
	std::cout<<"im here in handle events"<<"\n";
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
				std::cout<<"one client has been connected "<<"this is is his fd"<<client_fd<<"\n";
				registerEvents(client_fd,EVFILT_READ);
			}
			else if (event.filter == EVFILT_READ)
			{
				text = readLine(event.ident);
				if (text.empty())
				{
					close(event.ident);
					deregisterEvent(event.ident,EVFILT_READ);
					continue;
				}
				Client *client = getClientByFd(event.ident);
				if (client == NULL)
					clients_list[event.ident] = new Client(event.ident);
				toSend = determinCommandSide(text, *getClientByFd(event.ident));
				// this->authClient(*client);
				std::cout<<text<<"\n";
				std::cout<<toSend.size()<<"\n";
				registerChannelCients(toSend); 
			}
			else if (event.filter == EVFILT_WRITE)
			{
				Client *client = getClientByFd(event.ident);
				if (client && !client->getBuffer().empty())
				{
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

static std::vector<std::string> splitWords(const std::string msg)
{
	std::istringstream iss(msg);
	std::vector<std::string> words;
	std::string temp;

	while (iss >> temp)
		words.push_back(temp);
	return words;
}

std::vector <Client * > Server::parseChannelCommand(std::string message, Client & sender)
{
	this->authClient(sender);
	if (!sender.getAuth())
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"451: " + sender.getNick() + " :You have not registered\n");
	std::vector<std::string> command = splitWords(message);
	std::string commands[] = { "JOIN", "PRIVMSG", "INVITE", "TOPIC", "KICK", "MODE" };

	std::cout << std::endl;
	if (command[0] == "JOIN") // and # of params
		return (command.erase(command.begin()), handleJoin(command, sender));
	if (command[0] == "PRIVMSG")
	    return (command.erase(command.begin()), handlePrivMsg(message, command, sender));
	if (command[0] == "INVITE")
		return (command.erase(command.begin()), handleInvite(command, sender));
	if (command[0] == "TOPIC")
	    return (command.erase(command.begin()), handleTopic(command, sender));
	if (command[0] == "KICK")
		return (command.erase(command.begin()), handleKick(command, sender));
	if (command[0] == "MODE")
		return (command.erase(command.begin()), handleMode(command, sender));
	else
		{
			return setClientsBuffer(std::vector< Client*>(1, &sender),
			"421: " + sender.getNick() + " " + command[0] + " :Unknown command\n");}
}
bool Server::channelNameIsValid(const std::string &name)
{
	if (name.empty() || name.length() > 50)
		return false;
	for (size_t i = 1; i < name.length(); ++i)
	{
		if (name[i] == 0x20 || name[i] == 0x07 || name[i] == 0x2C)
			return false;
	}
	return true;
}
std::vector <Client * > Server::handleJoin(std::vector<std::string> command, Client & sender)
{
	if (command.size() < 1 || command.size() > 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n");
	if (command[0][0] == '#')
	{
		command[0].erase(0, 1);
		Channel *channel = getChannel(command[0]);
		if (channel == NULL)
		{
			if (!channelNameIsValid(command[0]))
				return setClientsBuffer(std::vector< Client*>(1, &sender),
					"403: " + sender.getNick() + " " + command[0] + " :No such channel\n");
			_channels[command[0]] = new Channel (command[0]); // we should send to the sender
			return _channels[command[0]]->init(&sender);
		}
		else
		{
			std::cout << "channel exists\n";
			if (command.size() == 2)
				return channel->addClient(& sender, command[1]);
			return channel->addClient(& sender);	
		}
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender),
		"403: " + sender.getNick() + " " + command[0] + " :No such channel\n");
}
	
std::vector <Client * > Server::handlePrivMsg(std::string msg, std::vector<std::string> command, Client & sender) // should I send the whole message? or without the client?
{
	if (command.size() == 0)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"411: " + sender.getNick() + " :No recipient given (PRIVMSG)\n");
	if (command[0][0] == '#')
	{
		command[0].erase(0, 1);
		Channel * channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"403: " + sender.getNick() + " " + command[0] + " :No such channel\n");
		return channel->sendToClients(msg, &sender);
	}
	else
	{
		Client * reciever = getClientByNick(command[0]);
		if (!reciever)
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"401: " + sender.getNick() + " " + command[0] + " :No such nick/channel\n");
		return setClientsBuffer(std::vector< Client*>(1, reciever),
			sender.getNick() + " : " + msg);		
	}

	// NICK NAME ERR_NOSUCHNICK (401) ERR_NORECIPIENT (411)
	
	// CHANNEL ERR_CANNOTSENDTOCHAN (404)
	// TEXT ERR_NOTEXTTOSEND (412)
}

std::vector <Client * > Server::handleInvite(std::vector<std::string> command, Client & sender)
{
	// INVITE NICK #CHANNEL
	
	if (command.size() != 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n");
	if (!getClientByNick(command[0]))
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"406: " + sender.getNick() + " " + command[0] + " :There was no such nickname\n");
	if (command[1][0] == '#')
	{
		command[1].erase(0, 1);
		Channel *channel = getChannel(command[1]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"403: " + sender.getNick() + " " + command[1] + " :No such channel\n");
		else
			return channel->inviteClient(&sender, getClientByNick(command[0]));
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender),
		"401: " + sender.getNick() + " " + command[0] + " :No such nick/channel\n");
}
	
std::vector <Client * > Server::handleTopic(std::vector<std::string> command, Client & sender)
{
	Channel * channel = NULL;
	if (command.size() == 0)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n");
	if (command[0][0] == '#')
	{
		std::cout << "this is a channel!\n";
		command[0].erase(0, 1);
		channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"403: " + sender.getNick() + " " + command[0] + " :No such channel\n");
		if	(command.size () == 1) // viewing a topic
			return channel->getTopic(&sender);
		else if (command.size() == 2) // changing a topic of a channel
			return channel->setTopic(&sender, command[1]);
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender),
		"401: " + sender.getNick() + " " + command[0] + " :No such nick/channel\n");
}

std::vector <Client * > Server::handleKick(std::vector<std::string> command, Client & sender)
{
	if (command.size() != 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n");
	if (command[0][0] == '#')
	{
		command[0].erase(0, 1);
		Channel *channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"403: " + sender.getNick() + " " + command[0] + " :No such channel\n");
		else if (!getClientByNick(command[1]))
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"406: " + sender.getNick() + " " + command[1] + " :There was no such nickname\n");
		else
			return channel->removeClient(&sender, getClientByNick(command[1]));
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender),
		"401: " + sender.getNick() + " " + command[0] + " :No such nick/channel\n");
}
	
std::vector <Client * > Server::handleMode(std::vector<std::string> command, Client & sender)
{
	if (command.size() < 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n");

	if (command[0][0] != '#')
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"401: " + sender.getNick() + " " + command[0] + " :No such nick/channel\n");
	command[0].erase(0, 1);
	Channel *channel = getChannel(command[0]);
	if (channel == NULL)
		return setClientsBuffer(std::vector< Client*>(1, &sender),
			"403: " + sender.getNick() + " " + command[0] + " :No such channel\n");
	if (command.size() == 2 && command[1].size() == 2)
	{
		if (command[1][0] == '+' && command[1][1] == 'i')
			return channel->setInviteOnly(&sender);
		if (command[1][0] == '+' && command[1][1] == 't')
			return channel->setTopicRestrict(&sender);
		if (command[1][0] == '-' && command[1][1] == 'i')
			return channel->unsetInviteOnly(&sender);
		if (command[1][0] == '-' && command[1][1] == 't')
			return channel->unsetTopicRestrict(&sender);
		if (command[1][0] == '-' && command[1][1] == 'k')
			return channel->unsetPassword(&sender);
		if (command[1][0] == '-' && command[1][1] == 'l')
			return channel->unsetUserLimit(&sender);
	}
	else if (command.size() == 3 && command[1].size() == 2)
	{
		if (command[1][0] == '+' && command[1][1] == 'k')
			return channel->setPassword(&sender, command[2]);
		if (command[1][0] == '+' && command[1][1] == 'o')
		{
			Client * client = getClientByNick(command[2]);
			if (client)
				return channel->addOperator(&sender, client);
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"406: " + sender.getNick() + " " + command[2] + " :There was no such nickname\n");
		}
		if (command[1][0] == '+' && command[1][1] == 'l')
			return channel->setUserLimit (&sender, atoi(command[2].c_str()));
		if (command[1][0] == '-' && command[1][1] == 'o')
		{
			Client * client = getClientByNick(command[2]);
			if (client)
				return channel->removeOperator(&sender, client);
			return setClientsBuffer(std::vector< Client*>(1, &sender),
				"406: " + sender.getNick() + " " + command[2] + " :There was no such nickname\n");
		}
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender),
		"472: " + sender.getNick() + " " + command[0] + " :is unknown mode char to me for channel\n");
}

std::vector <Client * >    Server::determinCommandSide(const std::string msg, Client &sender)
{
	
    std::vector<std::string> words = splitWords(msg);
    std::string error_msg = "421: " + sender.getName() + " " + words[0] + " " + ": Unknown Command.";
    std::string commands[] = { "JOIN", "PRIVMSG", "INVITE", "TOPIC", "KICK", "MODE", "NICK", "USER", "PASS", "INVALID" };
    int i = 0;
    while (i < 10)
    {
        if (commands[i] == words[0])
            break ;
		if (i == 9)
			break ;
        i++;
    }
    if (i == 9)
      { 
		 sender.setBuffer(error_msg);}
    return (std::cout<<"im here in determinCommandSide"<<"\n",(i >= 0 && i <= 5) ? this->parseChannelCommand(msg, sender) : (i >= 6 && i <= 8) ? this->parseClientCommand(words, sender) : std::vector< Client *>(1, &sender));
}

std::vector <Client * > Server::parseClientCommand(std::vector<std::string> msg, Client &sender)
{
    std::string commands[] = { "NICK", "USER", "PASS" };
    int i = 0;
	std::cout << "the command is: " << msg[0] << std::endl;
    while (i < 3)
    {
		if (commands[i] == msg[0])
		{
			std::cout << "the case is: " << i << std::endl;
				break ;
		}
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
    }
	return std::vector< Client *>(1, &sender);
}

std::vector <Client * > Server::setClientsBuffer(std::vector<Client * > clients, std::string message)
{
	for (std::vector<Client * >::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->setBuffer(message);
	}
	return clients;
}