#include "../inc/Client.hpp"

Client::Client(int _socketFd)
{
	this->socketFd = _socketFd;
	this->authenticated = false;
	this->registered = false;
	this->realname = "";
	this->username = "";
	this->nickname = "";
	this->password = "";
}

Client::Client(const Client &copy)
{
	this->nickname = copy.nickname;
	this->username = copy.username;
	this->realname = copy.realname;
	this->password = copy.password;
	this->authenticated = copy.authenticated;
	this->registered = copy.registered;
}

Client &Client::operator=(const Client &ref)
{
	this->nickname = ref.nickname;
	this->username = ref.username;
	this->realname = ref.realname;
	this->password = ref.password;
	this->authenticated = ref.authenticated;
	this->registered = ref.registered;
	return *this;
}

Client::~Client()
{
}
