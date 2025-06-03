__messages__ : An IRC message is a single line, delimited by a pair of CR ('\r', 0x0D) and LF ('\n', 0x0A) characters.
	- When reading messages from a stream, read the incoming data into a buffer. Only parse and process a message once you encounter the \r\n at the end of it. If you encounter an empty message, silently ignore it.
	- When sending messages, ensure that a pair of \r\n characters follows every single message your software sends out.

__commands:__
- JOIN #channel {password}
- PRIVMSG #channel/nickname message
	* if no message it will show 401: No suhc nick / 403: No such channel

__operators commands:__
- KICK #channel name
- INVITE #channel name (client have to join after invitation)
- TOPIC #channel new topic (can have spaces).
- MODE
	MODE #channel +k pass
	MODE #channel +i (in sara and sumaya it takes extra parameters)
	MODE #channel +o nickname
	MODE #channel +l  limti (461 MODE +l :invalid limit)
	MODE #channel +t topic

command class:
- command
- channel
- client 
- arguments (e.g. message)
- options (e.g. +i)

server:
- in the server: we will store channels with map container.
- the commands will be from the server, we will pass the client and the whole string of the command.

channel:
- name: start with name specifier, no comma + no space.
- Upon joining, clients are shown which types of channels the server supports with the CHANTYPES parameter.




# THINGS TO FIX
- When we create the channel -> the message that we send to the user
- I should not remove the hashtag in the channel name


# TESTING
* for all channel commands we have to first check that the clliennt is authenticated
1. JOIN
	- channel name syntax
	- Join a channel that already exists vs. a new one
	- can join
	- can't join
		* +i and not invited
		* +k and wrong password
		* +l and is full
	- Try joining multiple channels at once (e.g. JOIN #a,#b,#c)
	- Try joining multiple channels at once (e.g. JOIN #a,#b,#c)

2. PRIVMSG
	- can send a message to chnnel
	- can't send a message to channel
		* not a member of the channel
	- can send a message to a client
	- can't send a message to a client
		* reviever does not exist
	- send to yourself

3. KICK
	- commander is moderator
		* user does not exist
		* kick yourself
	- commander is not moderator
	- KICK/disconnect the last client

4. INVITE
	- commander is moderator
		* user does not exist
		* user is not authenticated
	- commander is not moderator

5. TOPIC
	- get topic vs set topic
	- commander is moderator
		* syntax of the topic
	- +t and commander is not moderator

6. MODE
	- commander is moderator
		* +i
		* -i
		* +t
		* -t
		* +k [password-syntax]
		* -k
			* when key is not set
		* +o
			* user is not a channel member
			* user is not a server member
		* -o
			* user is not a channel member
			* user is not a server member
		* +l
			- what if users are already more than the limit?
			- non-number limit
		* invallid flag
	- commander is not moderator
		