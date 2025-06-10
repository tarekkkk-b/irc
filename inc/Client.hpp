#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "Channel.hpp"
#include "Server.hpp"

class Server;

class Client
{
	private:
		int socketFd;
    	std::string nickname;
    	std::string username;
    	std::string realname;
		std::string prefix;
    	// std::string hostname;
		std::string buffer;
		bool checked;
    	bool authenticated;

    	std::vector<std::string> channels;

	public:
		Client();
		Client(int _socketFd);
		Client(const Client &copy);
		Client &operator=(const Client &ref);
		~Client();

		bool getAuth() const;
		std::string getUser() const;
		std::string getNick() const;
		std::string getName() const;
		std::string getBuffer() const;
		std::string getPrefix() const;
		bool getPass() const;
		int	getSocketFd() const;
		

		void setBuffer(const std::string &message);
		void setPrefix();
		void printWelcome();
		void welcoming();
		void clearBuffer();
		void setName(const std::string _name);

		std::vector < Client * > setUser(std::vector<std::string> _username, const Server &server);
		std::vector < Client * > setNick(const std::vector<std::string> _nickname, Server &server);
		std::vector < Client * > setPass(const std::vector<std::string> _pass, const Server &server);

		void destroyClient();
		std::vector<std::string> &getChannels();
};
