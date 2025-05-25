#include "../inc/Client.hpp"

Client::Client()
{
	this->socketFd = -1;
	this->authenticated = false;
	this->checked = false;
	this->realname = "";
	this->username = "";
	this->nickname = "";
}

Client::Client(int _socketFd)
{
	this->socketFd = _socketFd;
	this->authenticated = false;
	this->realname = "";
	this->username = "";
	this->nickname = "";
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
// ilysm uwu <3
std::string Client::getName() const
{
	return this->realname;
}

bool Client::getPass() const
{
	return this->checked;
}

int Client::getSocketFd() const
{
	return this->socketFd;
}

std::string Client::getBuffer() const
{
	return this->buffer;
}
void Client::setAuth(bool status)
{
	this->authenticated = status;
}

void Client::setBuffer(const std::string &message)
{
	if (!message.empty())
		this->buffer = message;
}
message Client::setUser(std::vector<std::string> _username, const Server &server)
{
	(void)server;
	std::string params = "461: " + this->getUser() +  "PASS: Not enough parameters.";
	std::string registered = "462: " +  this->getUser() + ": You may not reregister.";
	std::string _realname = "";

	this->buffer = "";
	if (!this->getUser().empty())
		this->buffer = registered;
	else if (_username.size() < 5)
		this->buffer = params;
	else
	{
		this->username = _username[1];
		for (size_t i = 4; i < _username.size(); i++)
		{
			if (_username[i][0] == ':')
				_username[i].erase(0, 1);
			_realname += _username[i];
		}
		this->setName(_realname);
	}
	return std::make_pair("", std::vector<const Client *>(1, this));
}

static bool checkInUse(std::string _nickname, Server &server)
{
	if (!server.getClientByNick(_nickname))
		return true;
	return false;
}

static bool checkValidNick(std::string _nickame)
{
	if (_nickame.length() > 9)
		return false;
	for (size_t i = 0; i < _nickame.length(); i++)
	{
		if (!(std::isalnum(_nickame[i])) && !(std::isalpha(_nickame[i])))
			return false;
	}
	return true;
}

message Client::setNick(const std::vector<std::string> _nickname, Server &server)
{
	std::string noNick = "431: " + this->getUser() + " : No nickname given";
	std::string erroneus = "432: " + this->getUser() + _nickname[1] + " : Erroneus nickname";
	std::string inUse = "433: " + this->getUser() + _nickname[1] + " : Nickname is already in use";

	this->buffer = "";
	if (_nickname.size() < 2)
		this->buffer = noNick;
	else if (checkInUse(_nickname[1], server))
		this->buffer = inUse;
	else if (!checkValidNick(_nickname[1]))
		this->buffer = erroneus;
	else
		this->nickname = _nickname[1];
	return std::make_pair("", std::vector<const Client *>(1, this));
}

void Client::setName(const std::string _name)
{
	this->realname = _name;
}

message Client::setPass(const std::vector<std::string> _pass, const Server &server)
{
	std::string params = "461: " + this->getUser() +  "PASS: Not enough parameters.";
	std::string registered = "462: " +  this->getUser() + ": You may not reregister.";
	std::string mismatch = "464: " + this->getUser() + " : Password incorrect.";

	if (this->checked)
	{
		this->buffer = registered;
		return std::make_pair("", std::vector<const Client *>(1, this));
	}
	if (_pass.size() == 2)
	{
		if (server.getServPass() == _pass[1])
			this->checked = true;
		else
			this->buffer = mismatch;
	}
	else
		this->buffer = params;
	return std::make_pair("", std::vector<const Client *>(1, this));
}

// void	Client::joinChannel(const std::vector<std::string> args, Server &server) const
// {
// 	if (args.size() == 3)
// 	{

// 	}
// }

// void Client::leaveChannel(const std::string _channelName) const
// {
// 	(void)_channelName;
// }

// static std::vector<std::string> splitWords(const std::string msg)
// {
// 	std::istringstream iss(msg);
// 	std::vector<std::string> words;
// 	std::string temp;

// 	while (iss >> temp)
// 		words.push_back(temp);
// 	return words;
// }

// void Client::parseUserCommands(const std::string msg)
// {
// 	if (msg.empty())
// 	{
// 		std::cerr << "empty message" << std::endl;
// 		return ;
// 	}
// 	std::vector<std::string> words = splitWords(msg);
// 	std::string commands[5] = {"/NICK", "/PASS", "/JOIN", "/NAME", "/USER"};
// 	int i = 0;
// 	while (i < 5)
// 	{
// 		if (words[0] == commands[i])
// 			break ;
// 		i++;
// 	}
// 	if (words.size() != 2)
// 		return ;
// 	switch (i)
// 	{
// 		case  0:
// 			this->setNick(words[1]);
// 			break;
// 		case  1:
// 			this->setPass(words[1]);
// 			break;
// 		// case  2:
// 		// 	this->joinChannel(words[1]);
// 		// 	break;
// 		case  3:
// 			this->setName(words[1]);
// 			break;
// 		case  4:
// 			this->setUser(words[1]);
// 			break;
// 	}
// }




// void check_commands(const std::string msg)
// {
// 	std::vector<std::string> arguments = splitWords(msg);
// 	std::string commands[5] = {"/NICK", "/PASS", "/JOIN", "/NAME", "/USER"};
	
// }