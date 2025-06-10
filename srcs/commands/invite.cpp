#include "Server.hpp"

std::vector <Client * > Server::handleInvite(std::vector<std::string> command, Client & sender)
{	
	std::string notEnoughParams = ":ircserver 461 " + sender.getNick() + " INVITE :Not enough parameters\r\n";
	if (command.size() != 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);
	std::string noSuchNick = ":ircserver 401 " + sender.getNick() + " " + command[0] + " :No such nick/channel\r\n";
	std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[0] + " :No such channel\r\n";
	std::string noSuchNickName = ":ircserver 406 " + sender.getNick() + " " + command[0] + " :There was no such nickname\r\n";
	if (!getClientByNick(command[0]))
		return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNickName);
	if (command[1][0] == '#')
	{
		std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[1] + " :No such channel\r\n";
		Channel *channel = getChannel(command[1]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		else
			return channel->inviteClient(&sender, getClientByNick(command[0]));
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
}