#include "../../inc/Server.hpp"

std::vector <Client * > Server::handleMode(std::vector<std::string> command, Client & sender)
{
	std::string notEnoughParams = "461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n";
	std::string noSuchNick = "401: " + sender.getNick() + " " + command[0] + " :No such nick\n";
	std::string noSuchChannel = "403: " + sender.getNick() + " " + command[0] + " :No such channel\n";
	std::string noSuchNickName = "406: " + sender.getNick() + " " + command[0] + " :There was no such nickname\n";
	std::string unknownChar = "472: " + sender.getNick() + " " + command[0] + " :is unknown mode char to me for channel\n";

	if (command.size() < 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);

	if (command[0][0] != '#')
		return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
	Channel *channel = getChannel(command[0]);
	if (channel == NULL)
		return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
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
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNickName);
		}
		if (command[1][0] == '+' && command[1][1] == 'l')
			return channel->setUserLimit (&sender, atoi(command[2].c_str()));
		if (command[1][0] == '-' && command[1][1] == 'o')
		{
			Client * client = getClientByNick(command[2]);
			if (client)
				return channel->removeOperator(&sender, client);
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNickName);
		}
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), unknownChar);
}