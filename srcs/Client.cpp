#include "../inc/Client.hpp"

Client::Client()
{
	this->socketFd = -1;
	this->authenticated = false;
	this->realname = "";
	this->username = "";
	this->nickname = "";
	this->password = "";
}

Client::Client(int _socketFd)
{
	this->socketFd = _socketFd;
	this->authenticated = false;
	this->realname = "";
	this->username = "";
	this->nickname = "";
	this->password = "";
}

Client::Client(const Client &copy)
{
	*this = copy;
}

Client &Client::operator=(const Client &ref)
{
	this->nickname = ref.nickname;
	this->username = ref.username;
	this->realname = ref.realname;
	this->password = ref.password;
	this->authenticated = ref.authenticated;
	return *this;
}

Client::~Client()
{
}

bool Client::getAuth() const
{
	return this->authenticated;
}

std::string Client::getUser() const
{
	return this->username;
}

std::string Client::getNick() const
{
	return this->nickname;
}

std::string Client::getName() const
{
	return this->realname;
}

std::string Client::getPass() const
{
	return this->password;
}
void Client::setAuth(bool status)
{
	this->authenticated = status;
}

void Client::setUser(const std::string _username)
{
	this->username = _username;
}

void Client::setNick(const std::string _nickname)
{
	this->nickname = _nickname;
}

void Client::setName(const std::string _name)
{
	this->realname = _name;
}

void Client::setPass(const std::string _pass)
{
	this->password = _pass;
}

void	Client::joinChannel(const std::vector<std::string> args, Server &server) const
{
	if (args.size() == 3)
	{

	}
}

// void Client::joinChannel(const std::string _channelName) const
// {
// 	(void)_channelName;
// }

void Client::leaveChannel(const std::string _channelName) const
{
	(void)_channelName;
}

static std::vector<std::string> splitWords(const std::string msg)
{
	std::istringstream iss(msg);
	std::vector<std::string> words;
	std::string temp;

	while (iss >> temp)
		words.push_back(temp);
	return words;
}

void Client::parseUserCommands(const std::string msg)
{
	if (msg.empty())
	{
		std::cerr << "empty message" << std::endl;
		return ;
	}
	std::vector<std::string> words = splitWords(msg);
	std::string commands[5] = {"/NICK", "/PASS", "/JOIN", "/NAME", "/USER"};
	int i = 0;
	while (i < 5)
	{
		if (words[0] == commands[i])
			break ;
		i++;
	}
	if (words.size() != 2)
		return ;
	switch (i)
	{
		case  0:
			this->setNick(words[1]);
			break;
		case  1:
			this->setPass(words[1]);
			break;
		// case  2:
		// 	this->joinChannel(words[1]);
		// 	break;
		case  3:
			this->setName(words[1]);
			break;
		case  4:
			this->setUser(words[1]);
			break;
	}
}




void check_commands(const std::string msg)
{
	std::vector<std::string> arguments = splitWords(msg);
	std::string commands[5] = {"/NICK", "/PASS", "/JOIN", "/NAME", "/USER"};
	
}