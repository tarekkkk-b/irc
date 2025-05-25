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

message Channel::init(Client const * channelCreator)
{
	if (_operators.size() != 0)
		return std::make_pair("client is already there!\n", std::vector<const Client*>(1, channelCreator));
	addClient(channelCreator);
	addOperator(channelCreator, channelCreator);
	return std::make_pair("channel created successfully\n", std::vector<const Client*>(1, channelCreator));
}
std::string Channel::getName() const
{
	return this->_name;
}

std::vector < Client const * > Channel::getRecievers(Client const * sender, int withSender)
{
	std::vector <Client const *> recievers;
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if ( !withSender && _clients[i]->getSocketFd() == sender->getSocketFd())
			continue;
		recievers.push_back(_clients[i]);
	}
	return recievers;
}

message    Channel::addClient(Client const * client, std::string password)
{
	if (clientIsMember(client))
		return std::make_pair("client is already there!\n", std::vector<const Client*>(1, client));
	if (this->hasUsersLimit && _clients.size() >= (unsigned long) usersLimit)
		return std::make_pair("471: <client> <channel> :Cannot join channel (+l)\n", std::vector<const Client*>(1, client));
	else if (this->isInviteOnly && !clientIsInvited(client))
		return std::make_pair("473: <client> <channel> :Cannot join channel (+i)\n",std::vector<const Client*>(1, client));
	else if (this->hasPassword && password != this->_password)
		return std::make_pair("475: <client> <channel> :Cannot join channel (+k)\n", std::vector<const Client*>(1, client));
		else
		{
			std::string message = std::string("<client->socketFd>") + std::string(" ") + this->_name + std::string(" :");
			if (clientIsInvited(client))
			uninviteClient(client);
			this->_clients.push_back(client);
			if (this->isTopicRestricted)
			message += this->_topic + "\n";
			else
				message += "No topic is set\n";
			message += "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}\n"; // list of clients in the future
			return std::make_pair(message, std::vector<const Client*>(1, client));
	}
}

message    Channel::removeClient(Client const * commander, Client const * client)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	std::vector<Client const * >::iterator clientToBeRemoved = std::find(_clients.begin(), _clients.end(), client);
	if (clientToBeRemoved != _clients.end())
		_clients.erase(clientToBeRemoved);

	clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
	if (clientToBeRemoved != _operators.end())
		_operators.erase(clientToBeRemoved);
	return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
}

message    Channel::inviteClient(Client const * commander, Client const * client)
{
	
	if (this->isInviteOnly && !clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :You're not channel operator\n", std::vector<const Client*>(1, commander));
	else if (!clientIsMember(commander))
		return std::make_pair("442: <client> <channel> :You're not on that channel\n", std::vector<const Client*>(1, commander));
	else if (clientIsMember(client))
		return std::make_pair("443: <client> <nick> <channel> :is already on channel", std::vector<const Client*>(1, commander));

	this->_invitations.push_back(client);
	return std::make_pair("341: <client> <nick> <channel>\n", std::vector<const Client*>(1, commander));
}

void    Channel::uninviteClient(Client const * client)
{
		std::vector<Client const * >::iterator invitationToBeRemoved = std::find(_invitations.begin(), _invitations.end(), client);
		if (invitationToBeRemoved != _invitations.end())
			_invitations.erase(invitationToBeRemoved);
}

message    Channel::addOperator(Client const * commander, Client const * client)
{
	if (clientIsOperator(commander) || _operators.size() == 0)
	{
		this->_operators.push_back(client);
		return std::make_pair(std::string ("client <nickname> is now an operator"), std::vector<const Client*>(1, commander));
	}
	return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
}

message    Channel::removeOperator(Client const * commander, Client const * client)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	std::vector<Client const* >::iterator clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
	if (clientToBeRemoved != _operators.end())
	{
		_operators.erase(clientToBeRemoved);
		return std::make_pair("operator removed successfully\n",std::vector<const Client*>(1, commander));
	}
	return std::make_pair("operator not found\n",std::vector<const Client*>(1, commander));
	
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

message	Channel::setTopic(Client const * commander, std::string topic) // does topic ahve special characters?
{
	if (!clientIsMember(commander))
		return std::make_pair("442: <client> <channel> :You're not on that channel\n", std::vector<const Client*>(1, commander));
	if (this->isTopicRestricted && !clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
	this->_topic = topic;
	return std::make_pair("332: <client> <channel> :<topic>\n", getRecievers(commander, 1)); // i SHOULD SEND TO COMMANDER TOO
}
message	Channel::getTopic(Client const * commander)
{
	if (!clientIsMember(commander))
		return std::make_pair("442: <client> <channel> :You're not on that channel\n", std::vector<const Client*>(1, commander));
	if (this->isTopicRestricted && !clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
	if (this->_topic == "")
		return std::make_pair("331: <client> <channel> :No topic is set\n", std::vector<const Client*>(1, commander));
	else
		return std::make_pair("332: <client> <channel> :<topic>\n", std::vector<const Client*>(1, commander)); // should send RPL_TOPICWHOTIME also
}

message	Channel::setTopicRestrict(Client const * commander)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	isTopicRestricted = true;
	return std::make_pair("<client> <channel> :channel is now topic restricted\n", std::vector<const Client*>(1, commander));
}

message	Channel::unsetTopicRestrict(Client const * commander)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	isTopicRestricted = false;
	return std::make_pair("<client> <channel> :no topic restiction\n", std::vector<const Client*>(1, commander));
}

message	Channel::setInviteOnly(Client const * commander)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
	isInviteOnly = true;
	return std::make_pair("<client> <channel> :set to invite only\n", std::vector<const Client*>(1, commander));
}
	
message	Channel::unsetInviteOnly(Client const * commander)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	isInviteOnly = false;
	return std::make_pair("<client> <channel> :no need for invite\n", std::vector<const Client*>(1, commander));
}

message	Channel::setUserLimit(Client const * commander, int limit)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	hasUsersLimit = true;
	usersLimit = limit;
	return std::make_pair("<client> <channel> :channel has a user limit\n", std::vector<const Client*>(1, commander));
}

message	Channel::unsetUserLimit(Client const * commander)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));

	hasUsersLimit = false;
	usersLimit = 0;
	return std::make_pair("<client> <channel> :channel has no user limit\n", std::vector<const Client*>(1, commander));
}

message	Channel::setPassword(Client const * commander, std::string password)
{
	// "<client> <target chan> :Key is not well-formed"
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
	else
	{
		hasPassword = true;
		this->_password = password;
		return std::make_pair("<client> <channel> :Channel is now password protected (+k)\n", std::vector<const Client*>(1, commander));
	}
}

message	Channel::unsetPassword(Client const * commander)
{
	if (!clientIsOperator(commander))
		return std::make_pair("482: <client> <channel> :Permission Denied- You're not channel operator\n", std::vector<const Client*>(1, commander));
	hasPassword = false;
	return std::make_pair("<client> <channel> :Channel is now open (-k)\n", std::vector<const Client*>(1, commander));
}
					
message Channel::sendToClients(std::string text, Client const * commander)
{
	if (!clientIsMember(commander))
		return std::make_pair("442: <client> <channel> :You're not on that channel\n", std::vector<const Client*>(1, commander));
	return std::make_pair(text, getRecievers(commander, 1)); // i SHOULD SEND TO COMMANDER TOO
}