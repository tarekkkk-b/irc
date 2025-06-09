#include "../../inc/Server.hpp"

std::vector <Client * > Server::handleInvite(std::vector<std::string> command, Client & sender)
{	
	std::string notEnoughParams = "461: " + sender.getNick() + " " + command[0] + " :Not enough parameters\n";
	std::string noSuchNick = "401: " + sender.getNick() + " " + command[0] + " :No such nick\n";
	std::string noSuchChannel = "403: " + sender.getNick() + " " + command[0] + " :No such channel\n";
	std::string noSuchNickName = "406: " + sender.getNick() + " " + command[0] + " :There was no such nickname\n";
	if (command.size() != 2)
	return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);
	if (!getClientByNick(command[0]))
	return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNickName);
	if (command[1][0] == '#')
	{
		Channel *channel = getChannel(command[1]);
		if (channel == NULL)
		return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		else
		return channel->inviteClient(&sender, getClientByNick(command[0]));
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
}