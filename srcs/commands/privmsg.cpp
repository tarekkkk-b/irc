#include "../../inc/Server.hpp"

std::vector <Client * > Server::handlePrivMsg(std::string msg, std::vector<std::string> command, Client & sender)
{
	std::string noSuchChannel = "403: " + sender.getNick() + " " + command[0] + " :No such channel\n";
	std::string noRecipient = "411: " + sender.getNick() + " :No recipient given (PRIVMSG)\n";
	std::string noSuchNick = "401: " + sender.getNick() + " " + command[0] + " :No such nick\n";
	std::string message = sender.getNick() + " :" + msg;
	if (command.size() == 0)
	return setClientsBuffer(std::vector< Client*>(1, &sender), noRecipient);
	if (command[0][0] == '#')
	{
		Channel * channel = getChannel(command[0]);
		if (channel == NULL)
		return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		return channel->sendToClients(msg, &sender);
	}
	else
	{
		Client * reciever = getClientByNick(command[0]);
		if (!reciever)
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
		return setClientsBuffer(std::vector< Client*>(1, reciever), message);		
	}
}