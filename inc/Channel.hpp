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

		std::vector < Client const * > _clients;
		std::vector <Client const * > _operators;
		std::vector < Client const * > _invitations;
		Channel();

	public:

		Channel(std::string const name);
		Channel(const  Channel & other);
		Channel & operator = (const Channel & rhs);
		~Channel();

		message init(Client const * channelCreator);
		std::string getName() const;
		std::vector < Client const * > getRecievers(Client const * sender, int withSender);
		
		message	addClient(Client const * client, std::string password = "");
		message	removeClient(Client const * commander, Client const * client);
		
		message	inviteClient(Client const * commander, Client const * client);
		void	uninviteClient(Client const * client);
		
		message	addOperator(Client const * commander, Client const * client);
		message	removeOperator(Client const * commander, Client const * client);
		
		message	setPassword(Client const * commander, std::string password); 
		message	unsetPassword(Client const * commander); 
		
		message	setTopic(Client const * commander, std::string topic); 
		message	getTopic(Client const * commander); 
		// message	unsetTopic(Client const * commander);

		message	setTopicRestrict(Client const * commander); 
		message	unsetTopicRestrict(Client const * commander); 

		message	setInviteOnly(Client const * commander); 
		message	unsetInviteOnly(Client const * commander);

		message	setUserLimit(Client const * commander, int limit);
		message	unsetUserLimit(Client const * commander);
		
		bool	clientIsMember(Client const *client) const;
		bool	clientIsOperator(Client const *client) const;
		bool	clientIsInvited(Client const *client) const;

		message sendToClients(std::string text, Client const * commander);
};

#endif