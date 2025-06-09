#include "../../inc/Server.hpp"

std::vector <Client * > Server::handleKick(std::vector<std::string> command, Client & sender)
{
	std::string notEnoughParams = "461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n";
	std::string noSuchNick = "401: " + sender.getNick() + " " + command[0] + " :No such nick\n";
	std::string noSuchChannel = "403: " + sender.getNick() + " " + command[0] + " :No such channel\n";
	std::string noSuchNickName = "406: " + sender.getNick() + " " + command[0] + " :There was no such nickname\n";

	if (command.size() != 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);
	if (command[0][0] == '#')
	{
		Channel *channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		else if (!getClientByNick(command[1]))
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNickName);
		else
			return channel->removeClient(&sender, getClientByNick(command[1]));
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
}