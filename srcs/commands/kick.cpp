# include "Server.hpp"

std::vector <Client * > Server::handleKick(std::vector<std::string> command, Client & sender)
{
	std::string notEnoughParams = ":ircserver 461 " + sender.getNick() + " KICK :Not enough parameters\r\n";
	if (command.size() != 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);

	std::string noSuchNick = ":ircserver 401 " + sender.getNick() + " " + command[0] + " :No such nick/channel\r\n";
	std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[0] + " :No such channel\r\n";
	std::string noSuchNickName = ":ircserver 406 " + sender.getNick() + " " + command[0] + " :There was no such nickname\r\n";

	if (command[0][0] == '#')
	{
		Channel *channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		else if (!getClientByNick(command[1]))
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNickName);
		else
		{
			std::vector <Client *> toSendTo = channel->removeClient(&sender, getClientByNick(command[1]));
			// if (channel->getNumberOfClients() == 0)
			// {
			// 	delete channel;
			// 	_channels.erase(command[0]);
			// }
			return toSendTo;
		}
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
}