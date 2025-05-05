# include "../inc/Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string name, Client * channelCreator) : isInviteOnly(false),
		isTopicRestricted(false), hasPassword(false), hasUsersLimit(false)
{
	// should I check the name here or in the command section??
	this->_name = name; // "476: <channel> :Bad Channel Mask"
	addClient(channelCreator, channelCreator);
	addOperator(channelCreator, channelCreator);
	std::cout << "Channel: " << name << " has been created by client: " << channelCreator->socketFd << '\n'; 
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
	// I might have to free all _clients and _operators
}

void    Channel::addClient(Client const * commander, Client * client, std::string password)
{
	
	if (this->hasUsersLimit && _clients.size() >= (unsigned long) usersLimit)
		std::cout << "471: <client> <channel> :Cannot join channel (+l)\n";
	else if (this->isInviteOnly && !clientIsOperator(commander))
		std::cout << "473: <client> <channel> :Cannot join channel (+i)\n";
	else if (this->hasPassword && password != this->_password)
		std::cout << "475: <client> <channel> :Cannot join channel (+k)\n";
	else
	{
		this->_clients.push_back(client);
		std::cout << client->socketFd << " " << this->_name << " :";
		if (this->isTopicRestricted)
			std::cout << this->_topic << "\n";
		else
			std::cout << "No topic is set\n";
	}

}
	
void    Channel::removeClient(Client const * commander, Client * client)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
	{
		std::vector<Client * >::iterator clientToBeRemoved = std::find(_clients.begin(), _clients.end(), client);
		if (clientToBeRemoved != _clients.end())
			_clients.erase(clientToBeRemoved);

		clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
		if (clientToBeRemoved != _operators.end())
			_operators.erase(clientToBeRemoved);
	}
}

void    Channel::addOperator(Client const * commander, Client * client)
{
	if (clientIsOperator(commander) || _operators.size() == 0)
		this->_operators.push_back(client);
	else
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
}

void    Channel::removeOperator(Client const * commander, Client * client)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
	{
		std::vector<Client * >::iterator clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
		if (clientToBeRemoved != _operators.end())
			_operators.erase(clientToBeRemoved);
	}
}

bool	Channel::clientIsMember(Client const *client) const
{
	if (std::find(_clients.begin(), _clients.end(), client) != _clients.end())
		return true;
	return false;
}

bool	Channel::clientIsOperator(Client const *client) const
{
	if (std::find(_operators.begin(), _operators.end(), client) != _operators.end())
		return true;
	return false;
}

void	Channel::setTopic(Client const * commander, std::string topic)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
	{
		isTopicRestricted = true;
		this->_topic = topic;
	}
}
void	Channel::unsetTopic(Client const * commander)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
		isTopicRestricted = false;
}
void	Channel::setInviteOnly(Client const * commander)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
		isInviteOnly = true;
}
	
void	Channel::unsetInviteOnly(Client const * commander)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
		isInviteOnly = false;
}

void	Channel::setPassword(Client const * commander, std::string password)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
	else
	{
		hasPassword = true;
		this->_password = password;
	}
}
void	Channel::unsetPassword(Client const * commander)
{
	if (!clientIsOperator(commander))
		std::cout << "482: <client> <channel> :Permission Denied- You're not channel operator\n"; // later to be changed to send()
}