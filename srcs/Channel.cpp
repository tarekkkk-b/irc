# include "Channel.hpp"

Channel::Channel(std::string const name) : isInviteOnly(false),
		isTopicRestricted(false), hasPassword(false), hasUsersLimit(false)
{
	this->_name = name;
	_topic = "";
}

Channel::~Channel()
{
	_clients.clear();
	_operators.clear();
}

std::vector <Client * > Channel::init(Client * channelCreator)
{
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
		if (!withSender && _clients[i]->getSocketFd() == sender->getSocketFd())
			continue;
		recievers.push_back(_clients[i]);
	}
	return recievers;
}

std::vector <Client * >	Channel::addClient(Client * client, std::string password)
{
	std::string clientExists = ":ircserver 443 " + client->getNick() + " " + this->_name + " :is already on channel\r\n";
	std::string fullChannel = ":ircserver 471 " + client->getNick() + " " + this->_name + " :Cannot join channel (+l)\r\n";
	std::string isInviteOnly = ":ircserver 473 " + client->getNick() + " " + this->_name + " :Cannot join channel (+i)\r\n";
	std::string hasPass = ":ircserver 475 " + client->getNick() + " " + this->_name + " :Cannot join channel (+k)\r\n";
	std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + _name + "\r\n";;
	std::string message = joinMsg;
	std::string topicReply = ":ircserver 332 " + client->getNick() + " " + this->_name + " :" + this->_topic + "\r\n";
	std::string names =   ":ircserver 353 " + client->getNick() + " = " + this->_name + " :";
	// :server 366 <nick> #channel :End of /NAMES list.
	std::string endOfNames =   ":ircserver 366 " + client->getNick() + " " + this->_name + " :End of /NAMES list.\r\n" ;
	
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
	if (!this->_topic.empty())
		message +=topicReply;
	message += names;
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if (clientIsOperator(_clients[i]))
			message += "@";
		message += _clients[i]->getNick();
		if ((unsigned long) i != _clients.size() - 1)
			message += " ";
	}
	message = message + "\r\n" + endOfNames;
	setClientsBuffer(std::vector< Client*>(1, client), message);
	setClientsBuffer(getRecievers(client, 0), joinMsg);
	return getRecievers(client, 1);
}

std::vector <Client * >    Channel::removeClient(Client * commander, Client * client)
{
	std::string notMember = "441: " + client->getNick() + " " + this->_name + " :They aren't on that channel\r\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";
	std::string clientExists = "443: " + client->getNick() + " " + this->_name + " :is already on channel\r\n";
	std::string fullChannel = "471: " + client->getNick() + " " + this->_name + " :Cannot join channel (+l)\r\n";
	std::string isInviteOnly = "473: " + client->getNick() + " " + this->_name + " :Cannot join channel (+i)\r\n";
	std::string hasPass = "475: " + client->getNick() + " " + this->_name + " :Cannot join channel (+k)\r\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string message = "client " + client->getNick() + " " + this->_name + " has been removed from channel\r\n";

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
	std::string invited = "341: " + commander->getNick() + " " + client->getNick() + " " + this->_name + " :has been invited to channel\r\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";
	std::string clientExists = "443: " + client->getNick() + " " + this->_name + " :is already on channel\r\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";

	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if (this->isInviteOnly && !clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
	else if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	else if (clientIsMember(client))
		return setClientsBuffer(std::vector<Client *>(1, commander), clientExists);

	this->_invitations.push_back(client);
	return setClientsBuffer(std::vector<Client *>(1, client), invited);
}

void    Channel::uninviteClient(Client * client)
{
		std::vector<Client * >::iterator invitationToBeRemoved = std::find(_invitations.begin(), _invitations.end(), client);
		if (invitationToBeRemoved != _invitations.end())
			_invitations.erase(invitationToBeRemoved);
}

std::vector <Client * >    Channel::addOperator(Client * commander, Client * client)
{
	std::string notMember = "441: " + client->getNick() + " " + this->_name + " :They aren't on that channel\r\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string isNowOperator = "client " + client->getNick() + " is now an operator\r\n";
	if (!clientIsMember(commander) && commander != client)
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if ((clientIsOperator(commander) && clientIsMember(client))|| _operators.size() == 0)
	{
		this->_operators.push_back(client);
		return setClientsBuffer(getRecievers(commander, 1), isNowOperator);
	}
	if ((clientIsOperator(commander) && !clientIsMember(client)))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMember);
	return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
}

std::vector <Client * >    Channel::removeOperator(Client * commander, Client * client)
{
	std::string notMember = "441: " + client->getNick() + " " + this->_name + " :They aren't on that channel\r\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string notOperator = 	"operator " + client->getNick() + " " + this->_name + " :is not an operator\r\n";	
	std::string noLongerOperator = 	"client " + client->getNick() + " is no longer an operator\r\n";

	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if ((clientIsOperator(commander) && !clientIsMember(client)))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMember);
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	std::vector<Client * >::iterator clientToBeRemoved = std::find(_operators.begin(), _operators.end(), client);
	if (clientToBeRemoved != _operators.end())
	{
		_operators.erase(clientToBeRemoved);
		return setClientsBuffer(getRecievers(commander, 1), noLongerOperator );
	}
	return setClientsBuffer(std::vector<Client *>(1, commander), noLongerOperator);
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
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string message = "332: " + commander->getNick() + " " + this->_name + " :" + topic + "\r\n";
	std::string noMessage = "331: " + commander->getNick() + " " + this->_name + " :No topic is set\r\n";
	
	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if (this->isTopicRestricted && !clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
	this->_topic = topic;
		return setClientsBuffer(std::vector<Client *>(1, commander), message);
}

std::vector <Client * >	Channel::getTopic(Client * commander)
{
	std::string message = "332: " + commander->getNick() + " " + this->_name + " :" + this->_topic + "\r\n";
	std::string noMessage = "331: " + commander->getNick() + " " + this->_name + " :No topic is set\r\n";
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";

	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
	if (this->_topic == "")
		return setClientsBuffer(std::vector<Client *>(1, commander), noMessage);
	else
		return setClientsBuffer(std::vector<Client *>(1, commander), message);
}

std::vector <Client * >	Channel::setTopicRestrict(Client * commander)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string topicRestricted = commander->getNick() + " " + this->_name + " :channel is now topic restricted\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	isTopicRestricted = true;
	return setClientsBuffer(std::vector<Client *>(1, commander), topicRestricted);
}

std::vector <Client * >	Channel::unsetTopicRestrict(Client * commander)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string notTopicRestricted = commander->getNick() + " " + this->_name + " :channel is now topic restricted\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	isTopicRestricted = false;
	return setClientsBuffer(std::vector<Client *>(1, commander), notTopicRestricted);
}

std::vector <Client * >	Channel::setInviteOnly(Client * commander)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string inviteOnly = commander->getNick() + " " + this->_name + " :channel is now invite only (+i)\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
	isInviteOnly = true;
	return setClientsBuffer(std::vector<Client *>(1, commander), inviteOnly);
}
	
std::vector <Client * >	Channel::unsetInviteOnly(Client * commander)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string notInviteOnly = commander->getNick() + " " + this->_name + " :channel is no longer invite only (-i)\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	isInviteOnly = false;
	return setClientsBuffer(std::vector<Client *>(1, commander), notInviteOnly);
}

std::vector <Client * >	Channel::setUserLimit(Client * commander, int limit)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string limitSet = commander->getNick() + " " + this->_name + " :channel user limit is now set\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	hasUsersLimit = true;
	usersLimit = limit;
	return setClientsBuffer(std::vector<Client *>(1, commander), limitSet);
}

std::vector <Client * >	Channel::unsetUserLimit(Client * commander)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string limitUnset = commander->getNick() + " " + this->_name + " :channel user limit is now unset\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);

	hasUsersLimit = false;
	usersLimit = 0;
	return setClientsBuffer(std::vector<Client *>(1, commander), limitUnset);
}

std::vector <Client * >	Channel::setPassword(Client * commander, std::string password)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string badKey = "475: " + commander->getNick() + " " + this->_name + " :Key is not well-formed\r\n";
	std::string keySet = commander->getNick() + " " + this->_name + " :Channel is now password protected (+k)\r\n";

	// "<client> <target chan> :Key is not well-formed"
	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
	if (password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos)
		return setClientsBuffer(std::vector<Client *>(1, commander), badKey);
	else
	{
		hasPassword = true;
		this->_password = password;
		return setClientsBuffer(std::vector<Client *>(1, commander), keySet);
	}
}

std::vector <Client * >	Channel::unsetPassword(Client * commander)
{
	std::string noPermission = "482: " + commander->getNick() + " " + this->_name + " :Permission Denied- You're not channel operator\r\n";
	std::string keyUnset = commander->getNick() + " " + this->_name + " :Channel is no longer password protected (-k)\r\n";

	if (!clientIsOperator(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), noPermission);
	hasPassword = false;
	return setClientsBuffer(std::vector<Client *>(1, commander), keyUnset);
}
					
std::vector <Client * > Channel::sendToClients(std::string text, Client * commander)
{
	std::string notMemberU = "442: " + commander->getNick() + " " + this->_name + " :You're not on that channel\r\n";

	if (!clientIsMember(commander))
		return setClientsBuffer(std::vector<Client *>(1, commander), notMemberU);
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

std::vector < Client * > Channel:: getClients()
{
	return(this-> _clients);
}

void Channel:: removeClientSilently( Client * client)
{
	if (!clientIsMember(client))
		return ;

	std::vector<Client * >::iterator clientToBeRemoved = std::find(_clients.begin(), _clients.end(), client);
	if (clientToBeRemoved != _clients.end())
		_clients.erase(clientToBeRemoved);

}

