# include "Client.hpp"

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
std::string Client::getPrefix() const
{
	return this->prefix;
}
void Client::setAuth(bool status)
{
	this->authenticated = status;
}

void Client::setBuffer(const std::string &message)
{
	if (!message.empty())
		this->buffer = message;
	else
		this->buffer = "";
}
void Client::setPrefix()
{
	if (this->authenticated)
	{
		this->prefix = ":" + this->getNick() + "!" + this->getUser() + "@localhost"; 
	}
}
void Client:: clearBuffer()
{
	this->buffer.clear();
}
std::vector < Client * > Client::setUser(std::vector<std::string> _username, const Server &server)
{
	(void)server;
	std::string params = "461: " + this->getUser() +  "USER: Not enough parameters.\n";
	std::string registered = "462: " +  this->getUser() + ": You may not reregister.\n";
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
			if (i != 4 && i != _username.size())
				_realname += " ";
			_realname += _username[i];
		}
		this->setName(_realname);
	}
	return std::vector< Client *>(1, this);
}

static bool checkInUse(std::string _nickname, Server &server)
{
	if (server.getClientByNick(_nickname))
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

std::vector < Client * > Client::setNick(const std::vector<std::string> _nickname, Server &server)
{
	std::string noNick = "431: No nickname given\n";
	std::string erroneus = "432: "  + _nickname[1] + " : Erroneus nickname\n";
	std::string inUse = "433: "  + _nickname[1] + " : Nickname is already in use\n";

	this->buffer = "";
	if (_nickname.size() < 2)
		this->buffer = noNick;
	else if (checkInUse(_nickname[1], server))
		this->buffer = inUse;
	else if (!checkValidNick(_nickname[1]))
		this->buffer = erroneus;
	else
		this->nickname = _nickname[1];
	return std::vector< Client *>(1, this);
}

void Client::setName(const std::string _name)
{
	this->realname = _name;
}

std::vector < Client * > Client::setPass(const std::vector<std::string> _pass, const Server &server)
{
	std::string params = "461: PASS: Not enough parameters.\n";
	std::string registered = "462: You may not reregister.\n";
	std::string mismatch = "464: Password incorrect.\n";

	if (this->checked)
	{
		this->buffer = registered;
		return std::vector< Client *>(1, this);
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
	return std::vector< Client *>(1, this);
}


void Client:: destroyClient()
{
	this->checked = 0;
	authenticated = 0;
	close (socketFd);
	buffer.clear();
	this->realname = "";
	this->username = "";
	this->nickname = "";
	this->socketFd = -1;
}
