# include "Server.hpp"

bool Server::channelNameIsValid(const std::string &name)
{
	if (name.empty() || name.length() > 50)
		return false;
	for (size_t i = 1; i < name.length(); ++i)
	{
		if (name[i] == 0x20 || name[i] == 0x07 || name[i] == 0x2C)
			return false;
	}
	return true;
}

std::vector <Client * > Server::handleJoin(std::vector<std::string> command, Client & sender)
{
	std::string notEnoughParams = ":ircserver 461 " + sender.getNick() + " JOIN :Not enough parameters\r\n";
	if (command.size() < 1 || command.size() > 2)
		return setClientsBuffer(std::vector< Client*>(1, &sender), notEnoughParams);
	std::string noSuchChannel = ":ircserver 403 " + sender.getNick() + " " + command[0] + " :No such channel\r\n";
	std::string badMask = ":ircserver 476 " + sender.getNick() + " " + command[0] + " :Bad Channel\r\n";
	if (command[0][0] == '#' && command[0].size() > 1)
	{
		Channel *channel = getChannel(command[0]);
		if (channel == NULL)
		{
			if (!channelNameIsValid(command[0]))
				return setClientsBuffer(std::vector< Client*>(1, &sender), badMask);
			_channels[command[0]] = new Channel (command[0]);
			sender.getChannels().push_back(command[0]); 
			return _channels[command[0]]->init(&sender);
		}
		else
		{
			sender.getChannels().push_back(command[0]); 
			if (command.size() == 2)
				return channel->addClient(& sender, command[1]);
			return channel->addClient(& sender);	
		}
	}
	return setClientsBuffer(std::vector< Client*>(1, &sender), badMask);
}