# include "../inc/Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string const name) : isInviteOnly(false),
		isTopicRestricted(false), hasPassword(false), hasUsersLimit(false)
{
	// should I check the name here or in the command section?? I should check in command, we should assume her that it is valid
	this->_name = name; // "476: <channel> :Bad Channel Mask"
	_topic = "";
}

Channel::Channel(const Channel & other)
{
	*this = other;
}

Channel &Channel::operator=(const Channel & rhs)
{
	this->_name = rhs._name;
	// are we okay copying pointers?
	this->_clients = rhs._clients;
	this->_operators = rhs._operators;
	return (*this);
}

Channel::~Channel()
{
	_clients.clear();
	_operators.clear();
	// I might have to free all _clients and _operators
}

std::vector <Client * > Channel::init(Client * channelCreator)
{
	if (_operators.size() != 0)
		return setClientsBuffer(std::vector<Client *>(1, channelCreator),
			"channel already exists\n");
	addOperator(channelCreator, channelCreator);
	return addClient(channelCreator);
}
std::string Channel::getName() const
{
	return this->_name;
}

std::vector < Client * > Channel::getRecievers(Client * sender, int withSender)
{
	std::vector <Client * > recievers;
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if ( !withSender && _clients[i]->getSocketFd() == sender->getSocketFd())
			continue;
		recievers.push_back(_clients[i]);
	}
	return recievers;
}

std::vector <Client * >    Channel::addClient(Client * client, std::string password)
{
	std::string clientExists = "443: " + client->getNick() + " " + this->_name + " :is already on channel\n";
	std::string fullChannel = "471: " + client->getNick() + " " + this->_name + " :Cannot join channel (+l)\n";
	std::string isInviteOnly = "473: " + client->getNick() + " " + this->_name + " :Cannot join channel (+i)\n";
	std::string hasPass = "475: " + client->getNick() + " " + this->_name + " :Cannot join channel (+k)\n";
	std::string message;

	if (clientIsMember(client))
		return setClientsBuffer(std::vector<Client *>(1, client), clientExists);
	if (this->hasUsersLimit && _clients.size() >= (unsigned long) usersLimit)
		return setClientsBuffer(std::vector<Client *>(1, client), fullChannel);
	if (this->isInviteOnly && !clientIsInvited(client))
		return setClientsBuffer(std::vector<Client *>(1, client), isInviteOnly);
	if (this->hasPassword && password != this->_password)
		return setClientsBuffer(std::vector<Client *>(1, client), hasPass);

	if (clientIsInvited(client))
		uninviteClient(client);
	this->_clients.push_back(client);
	message += client->getNick() + " " + this->_name + " :has joined the channel\n";
	if (!this->_topic.empty())
		message += client->getNick() + " " + this->_name + " :" + this->_topic + "\n";
	message += client->getNick() + " " + this->getName() + " :";
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if (clientIsOperator(_clients[i]))
			message += "@";
		message += _clients[i]->getNick();
		if ((unsigned long) i != _clients.size() - 1)
			message += " ";
	}
	message += "\n";
	setClientsBuffer(std::vector< Client*>(1, client), message);
	setClientsBuffer(getRecievers(client, 0), client->getNick() + " " + this->_name + " has joined the channel\n");
	return getRecievers(client, 1);
}

std::vector <Client * >    Channel::removeClient(Client * commander, Client * client)
{
	std::string notMember = "441: " + client->getNick() + " " + this->_name + " :They aren't on that channel\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n";
	std::string clientExists = "443: " + client->getNick() + " " + this->_name + " :is already on channel\n";
	std::string fullChannel = "471: " + client->getNick() + " " + this->_name + " :Cannot join channel (+l)\n";
	std::string isInviteOnly = "473: " + client->getNick() + " " + this->_name + " :Cannot join channel (+i)\n";
	std::string hasPass = "475: " + client->getNick() + " " + this->_name + " :Cannot join channel (+k)\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n";
	std::string message = "client " + client->getNick() + " " + this->_name + " has been removed from channel\n";

	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if (!clientIsMember(client))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMember);
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	std::vector<Client * >::iterator clientToBeRemoved = std::find(_clients.begin(), _clients.end(), client);
	if (clientToBeRemoved != _clients.end())
		_clients.erase(clientToBeRemoved);

	clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
	if (clientToBeRemoved != _operators.end())
		_operators.erase(clientToBeRemoved);
	return setClientsBuffer(std::vector<Client *>(1, commander), message);
}

std::vector <Client * >    Channel::inviteClient(Client * commander, Client * client)
{
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n";
	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if (this->isInviteOnly && !clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");
	else if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n");
	else if (clientIsMember(client))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"443: " + client->getNick() + " " + this->_name + " :is already on channel\n");

	this->_invitations.push_back(client);
	return setClientsBuffer(std::vector<Client *>(1, client),
			"341: " + commander->getNick() + " " + client->getNick() + " " + this->_name + " :has been invited to channel\n");
}

void    Channel::uninviteClient(Client * client)
{
		std::vector<Client * >::iterator invitationToBeRemoved = std::find(_invitations.begin(), _invitations.end(), client);
		if (invitationToBeRemoved != _invitations.end())
			_invitations.erase(invitationToBeRemoved);
}

std::vector <Client * >    Channel::addOperator(Client * commander, Client * client)
{
	std::string notMember = "441: " + client->getNick() + " " + this->_name + " :They aren't on that channel\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n";
	if (!clientIsMember(commander) && commander != client)
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if ((clientIsOperator(commander) && clientIsMember(client))|| _operators.size() == 0)
	{
		this->_operators.push_back(client);
		return setClientsBuffer(getRecievers(commander, 1),
			"client " + client->getNick() + " is now an operator\n");
	}
	if ((clientIsOperator(commander) && !clientIsMember(client)))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMember);

	return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");
}

std::vector <Client * >    Channel::removeOperator(Client * commander, Client * client)
{
	std::string notMember = "441: " + client->getNick() + " " + this->_name + " :They aren't on that channel\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n";
	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if ((clientIsOperator(commander) && !clientIsMember(client)))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMember);
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");

	std::vector<Client * >::iterator clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
	if (clientToBeRemoved != _operators.end())
	{
		_operators.erase(clientToBeRemoved);
		return setClientsBuffer(getRecievers(commander, 1),
			"client " + client->getNick() + " is no longer an operator\n");
	}
	return setClientsBuffer(std::vector<Client *>(1, commander),
			"operator " + client->getNick() + " " + this->_name + " :is not an operator\n");	
}

bool	Channel::clientIsMember(Client const * client) const
{
	if (std::find(this->_clients.begin(), this->_clients.end(), client) != this->_clients.end())
		return true;
	return false;
}

bool	Channel::clientIsOperator(Client const *client) const
{
	if (std::find(this->_operators.begin(), this->_operators.end(), client) != this->_operators.end())
		return true;
	return false;
}

bool	Channel::clientIsInvited(Client const *client) const
{
	if (std::find(_invitations.begin(), _invitations.end(), client) != _invitations.end())
		return true;
	return false;
}

std::vector <Client * >	Channel::setTopic(Client * commander, std::string topic) // does topic ahve special characters?
{
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n";
	std::string message = "332: " + commander->getNick() + " " + this->_name + " :" + topic + "\n";
	std::string noMessage = "331: " + commander->getNick() + " " + this->_name + " :No topic is set\n";
	
	if (!clientIsMember(commander))
	return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if (this->isTopicRestricted && !clientIsOperator(commander))
	return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
	this->_topic = topic;
	return setClientsBuffer(std::vector<Client *>(1, commander), message);
}

std::vector <Client * >	Channel::getTopic(Client * commander)
{
	std::string message = "332: " + commander->getNick() + " " + this->_name + " :" + this->_topic + "\n";
	std::string noMessage = "331: " + commander->getNick() + " " + this->_name + " :No topic is set\n";
	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n");
	if (this->_topic == "")
		return setClientsBuffer(std::vector<Client *>(1, commander), noMessage);
	else
		return setClientsBuffer(std::vector<Client *>(1, commander), message);
}

std::vector <Client * >	Channel::setTopicRestrict(Client * commander)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");

	isTopicRestricted = true;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :channel is now topic restricted\n");
}

std::vector <Client * >	Channel::unsetTopicRestrict(Client * commander)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");

	isTopicRestricted = false;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :channel is no longer topic restricted\n");
}

std::vector <Client * >	Channel::setInviteOnly(Client * commander)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");
	isInviteOnly = true;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :channel is now invite only (+i)\n");
}
	
std::vector <Client * >	Channel::unsetInviteOnly(Client * commander)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");

	isInviteOnly = false;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :channel is no longer invite only (-i)\n");
}

std::vector <Client * >	Channel::setUserLimit(Client * commander, int limit)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");

	hasUsersLimit = true;
	usersLimit = limit;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :channel user limit is now set\n");
}

std::vector <Client * >	Channel::unsetUserLimit(Client * commander)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");

	hasUsersLimit = false;
	usersLimit = 0;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :channel user limit is now unset\n");
}

std::vector <Client * >	Channel::setPassword(Client * commander, std::string password)
{
	// "<client> <target chan> :Key is not well-formed"
	if (password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos)
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"475: " + commander->getNick() + " " + this->_name + " :Key is not well-formed\n");
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");
	else
	{
		hasPassword = true;
		this->_password = password;
		return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :Channel is now password protected (+k)\n");
	}
}

std::vector <Client * >	Channel::unsetPassword(Client * commander)
{
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\n");
	hasPassword = false;
	return setClientsBuffer(std::vector<Client *>(1, commander),
			commander->getNick() + " " + this->_name + " :Channel is no longer password protected (-k)\n");
}
					
std::vector <Client * > Channel::sendToClients(std::string text, Client * commander)
{
	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander),
			"442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\n");
	return setClientsBuffer(getRecievers(commander, 0), text);
}

std::vector <Client * > Channel::setClientsBuffer(std::vector<Client * > clients, std::string message)
{
	for (std::vector<Client * >::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->setBuffer(message);
	}
	return clients;
}

