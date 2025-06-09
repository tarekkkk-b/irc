#include "../../inc/Server.hpp"

std::vector <Client * > Server::handleTopic(std::vector<std::string> command, Client & sender)
{
	
	if (command.size() == 0)
		return setClientsBuffer(std::vector< Client*>(1, &sender), "Not enough parameters\n");
	std::string notEnoughParams = "461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n";
	std::string noSuchNick = "401: " + sender.getNick() + " " + command[0] + " :No such nick\n";
	std::string noSuchChannel = "403: " + sender.getNick() + " " + command[0] + " :No such channel\n";
	std::string noSuchNickName = "406: " + sender.getNick() + " " + command[0] + " :There was no such nickname\n";
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