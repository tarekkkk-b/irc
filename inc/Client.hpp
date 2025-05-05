#pragma once
#include <iostream>

class Client
{
	private:
	// std::string hostname;                // Host of the client (optional)
	// std::string ipAddress;     //mand         // IP address (optional, useful for info)
	std::string nickname;      //on start         // Set via NICK command
	std::string username;       // on start        // Set via USER command
	std::string realname;       // on start        // From USER command (after :)
	std::string password;       // on start        // From PASS command
	bool authenticated;    //once all is recieved       // True only if PASS + NICK + USER are all sent
	bool registered;       // optional             // True after full registration (can join channels)
	// bool operatorMode;                  // Whether the user is an operator (for channels)
	
	// std::string buffer;                 // For accumulating partial messages
	// std::vector<std::string> channels;  // Names of channels this user is part of
	
	public:
		int socketFd;       //mand                 // Unique socket for communication
		Client(int _socketFd);
		Client(const Client &copy);
		Client &operator=(const Client &ref);
		~Client();
};
