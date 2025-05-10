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