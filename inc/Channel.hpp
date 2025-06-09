#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include "Client.hpp"
# include "types.hpp"

class Channel
{
	private:
		std::string _name;
		std::string _password;
		std::string _topic;
		int usersLimit;

		bool isInviteOnly;
		bool isTopicRestricted;
		bool hasPassword;
		bool hasUsersLimit;

		std::vector < Client * > _clients;
		std::vector <Client * > _operators;
		std::vector < Client * > _invitations;
		
		Channel();
		Channel(const  Channel & other);
		Channel & operator = (const Channel & rhs);
		
		public:
		
		Channel(std::string const name);
		~Channel();

		std::vector <Client * > init(Client * channelCreator);
		std::string getName() const;
		std::vector < Client * > getRecievers(Client * sender, int withSender);
		
		std::vector < Client * > addClient(Client * client, std::string password = "");
		std::vector < Client * > removeClient(Client * commander, Client * client);
		
		std::vector < Client * > inviteClient(Client * commander, Client * client);
		void	uninviteClient(Client * client);
		
		std::vector < Client * > addOperator(Client * commander, Client * client);
		std::vector < Client * > removeOperator(Client * commander, Client * client);
		
		std::vector < Client * > setPassword(Client * commander, std::string password); 
		std::vector < Client * > unsetPassword(Client * commander); 
		
		std::vector < Client * > setTopic(Client * commander, std::string topic); 
		std::vector < Client * > getTopic(Client * commander); 

		std::vector < Client * > setTopicRestrict(Client * commander); 
		std::vector < Client * > unsetTopicRestrict(Client * commander); 

		std::vector < Client * > setInviteOnly(Client * commander); 
		std::vector < Client * > unsetInviteOnly(Client * commander);

		std::vector < Client * > setUserLimit(Client * commander, int limit);
		std::vector < Client * > unsetUserLimit(Client * commander);
		
		bool	clientIsMember(Client const *client) const;
		bool	clientIsOperator(Client const *client) const;
		bool	clientIsInvited(Client const *client) const;

		std::vector < Client * > sendToClients(std::string text, Client * commander);
		std::vector <Client * > setClientsBuffer(std::vector<Client * > clients, std::string message);

};

#endif