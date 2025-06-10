# include "Server.hpp"

std::vector <Client * > Server::handleTopic(std::vector<std::string> command, Client & sender)
{
	
	std::string notEnoughParams = ":ircserver 461 " + sender.getNick() + " TOPIC :Not enough parameters\r\n";
	if (command.size() == 0)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);
	std::string noSuchNick = ":ircserver 401 " + sender.getNick() + " " + command[0] + " :No such nick/channel\r\n";
	std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[0] + " :No such channel\r\n";
	std::string noSuchNickName = ":ircserver 406 " + sender.getNick() + " " + command[0] + " :There was no such nickname\r\n";
	Channel * channel = NULL;
	if (command[0][0] == '#')
	{
		channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		if	(command.size () == 1)
			return channel->getTopic(&sender);
		else if (command.size() == 2)
			return channel->setTopic(&sender, command[1]);
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
}