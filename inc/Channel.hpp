#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include "../inc/Client.hpp"

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

		Channel(std::string const name, Client const * channelCreator);
		Channel(const  Channel & other);
		Channel & operator = (const Channel & rhs);
		~Channel();
		
		void    addClient(Client const * commander, Client const * client, std::string password = "");
		void    removeClient(Client const * commander, Client const * client);
		
		void    inviteClient(Client const * commander, Client const * client);
		void    uninviteClient(Client const * commander, Client const * client);
		
		void    addOperator(Client const * commander, Client const * client);
		void    removeOperator(Client const * commander, Client const * client);
		
		void	setPassword(Client const * commander, std::string password); 
		void	unsetPassword(Client const * commander); 
		
		void	setTopic(Client const * commander, std::string topic); 
		void	unsetTopic(Client const * commander);

		void	setInviteOnly(Client const * commander); 
		void	unsetInviteOnly(Client const * commander);
		
		bool	clientIsMember(Client const *client) const;
		bool	clientIsOperator(Client const *client) const;
};

#endif