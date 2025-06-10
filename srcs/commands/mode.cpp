# include "Server.hpp"

std::vector <Client * > Server::handleMode(std::vector<std::string> command, Client & sender)
{
	std::string notEnoughParams = ":ircserver 461 " + sender.getNick() + " MODE :Not enough parameters\r\n";
	if (command.size() < 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);

	std::string noSuchNick = ":ircserver 401 " + sender.getNick() + " " + command[0] + " :No such nick/channel\r\n";
	std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[0] + " :No such channel\r\n";
	std::string noSuchNickName = ":ircserver 406 " + sender.getNick() + " " + command[0] + " :There was no such nickname\r\n";
	std::string unknownChar = ":ircserver 472 " + sender.getNick() + " " + command[1] + " :is unknown mode char to me\r\n";

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