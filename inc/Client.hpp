#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "Channel.hpp"
#include "Server.hpp"

class Client
{
	private:
		int socketFd;       	//mand                 // Unique socket for communication
    	std::string nickname;	//on start         // Set via NICK command
    	std::string username;	// on start        // Set via USER command
    	std::string realname;	// on start        // From USER command (after :)
    	std::string hostname;	// on start        // From USER command (after :)
    	std::string password;	// on start        // From PASS command
    	bool authenticated;		//once all is recieved       // True only if PASS + NICK + USER are all sent
    	// bool registered;       // optional             // True after full registration (can join channels)
    	// bool operatorMode;                  // Whether the user is an operator (for channels)

    	// std::string buffer;                 // For accumulating partial messages
    	std::vector<std::string> channels;  // Names of channels this user is part of

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
		std::string getPass() const;
		
		void setAuth(bool status);
		void setUser(const std::string _username);
		void setNick(const std::string _nickname);
		void setName(const std::string _name);
		void setPass(const std::string _pass);

		// void joinChannel(const std::string _channelName) const;
		void joinChannel(const std::vector<std::string> args, Server &server) const;
		void leaveChannel(const std::string _channelName) const;

		void parseUserCommands(const std::string msg);
};
