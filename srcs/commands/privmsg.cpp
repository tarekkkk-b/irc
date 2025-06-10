# include "../../inc/Server.hpp"

std::vector <Client * > Server::handlePrivMsg(std::string msg, std::vector<std::string> command, Client & sender)
{
	std::cout <<"THE MESSAGE IS -> "<<msg<<"\n";
	std::string noRecipient = ":ircserver 411 " + sender.getNick() + " :No recipient given (PRIVMSG)\r\n";
	if (command.size() == 0)
		return setClientsBuffer(std::vector< Client*>(1, &sender), noRecipient);
	std::string noMsg = ":ircserver 412 " + sender.getNick() + " :No text to send\r\n";
	std::string noSuchNick = ":ircserver 401 " + sender.getNick() + " " + command[0] + " :No such nick\r\n";
	std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[0] + " :No such channel\r\n";
	size_t colonPos = msg.find(" :");
	std::string message;
	if (colonPos != std::string::npos)
		message = ":" + sender.getPrefix() + " PRIVMSG " + command[0] + " :" + msg.substr(colonPos + 2) + "\r\n";
	else
		return setClientsBuffer(std::vector< Client*>(1, &sender), noMsg);
	if (command[0][0] == '#')
	{
		Channel * channel = getChannel(command[0]);
		if (channel == NULL)
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchChannel);
		return channel->sendToClients(message, &sender);
	}
	else
	{
		Client * reciever = getClientByNick(command[0]);
		if (!reciever || !reciever->getAuth())
			return setClientsBuffer(std::vector< Client*>(1, &sender), noSuchNick);
		return setClientsBuffer(std::vector< Client*>(1, reciever), message);		
	}
}