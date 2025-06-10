# include "Server.hpp"

static std::vector<std::string> splitWords(const std::string msg)
{
	std::istringstream iss(msg);
	std::vector<std::string> words;
	std::string temp;

	while (iss >> temp)
		words.push_back(temp);
	return words;
}

std::vector <Client * >    Server::determinCommandSide(const std::string msg, Client &sender)
{
    std::vector<std::string> words = splitWords(msg);
	std::string nick = sender.getNick().empty() ? "*" : sender.getNick();
	std::string error_msg;
	error_msg = ":ircserver 421 " + nick + " " + words[0] + " :Unknown Command\r\n";
    std::string commands[] = { "JOIN", "PRIVMSG", "INVITE", "TOPIC", "KICK", "MODE", "NICK", "USER", "PASS", "INVALID" };
    int i = 0;
    while (i < 10)
    {
        if (commands[i] == words[0])
            break ;
		if (i == 9)
			break ;
        i++;
    }
    if (i == 9)
		sender.setBuffer(error_msg);
    return ((i >= 0 && i <= 5) ? this->parseChannelCommand(msg, sender) : (i >= 6 && i <= 8) ? this->parseClientCommand(words, sender) : std::vector< Client *>(1, &sender));
}

std::vector <Client * > Server::parseClientCommand(std::vector<std::string> msg, Client &sender)
{
    std::string commands[] = { "NICK", "USER", "PASS" };
    int i = 0;
    while (i < 3)
    {
		if (commands[i] == msg[0])
			break ;
        i++;
    }
    switch (i)
    {
    case 0:
        return (sender.setNick(msg, *this));
    case 1:
        return (sender.setUser(msg, *this));
    case 2:
        return (sender.setPass(msg, *this));
    }
	return std::vector< Client *>(1, &sender);
}

std::vector <Client * > Server::parseChannelCommand(std::string message, Client & sender)
{
	std::vector<std::string> command = splitWords(message);
	std::string unknownCommand = "421: " + sender.getNick() + " " + command[0] + " :Unknown command\n";
	std::string notRegistered = "451: " + sender.getNick() + " :You have not registered\n";
	if (!sender.getAuth())
		return setClientsBuffer(std::vector< Client*>(1, &sender), notRegistered);
	std::string commands[] = { "JOIN", "PRIVMSG", "INVITE", "TOPIC", "KICK", "MODE" };

	std::cout << std::endl;
	if (command[0] == "JOIN") // and # of params
		return (command.erase(command.begin()), handleJoin(command, sender));
	if (command[0] == "PRIVMSG")
	    return (command.erase(command.begin()), handlePrivMsg(message, command, sender));
	if (command[0] == "INVITE")
		return (command.erase(command.begin()), handleInvite(command, sender));
	if (command[0] == "TOPIC")
	    return (command.erase(command.begin()), handleTopic(command, sender));
	if (command[0] == "KICK")
		return (command.erase(command.begin()), handleKick(command, sender));
	if (command[0] == "MODE")
		return (command.erase(command.begin()), handleMode(command, sender));
	else
		return setClientsBuffer(std::vector< Client*>(1, &sender), unknownCommand);
}