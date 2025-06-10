# __messages__ : An IRC message is a single line, delimited by a pair of CR ('\r', 0x0D) and LF ('\n', 0x0A) characters.
	- When reading messages from a stream, read the incoming data into a buffer. Only parse and process a message once you encounter the \r\n at the end of it. If you encounter an empty message, silently ignore it.
	- When sending messages, ensure that a pair of \r\n characters follows every single message your software sends out.

# __commands:__
- JOIN #channel {password}
- PRIVMSG #channel/nickname message
	* if no message it will show 401: No suhc nick / 403: No such channel

# __operators commands:__
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
- [client] RPL_WELCOME (001)  : "<client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
- [server] when a client disconnect -> 1. remove from channels 2. remve from server 3. free memory
	* when we dont remove the client and close the fd only, this causes the new client to join in the old client's place
	* when we send to a channel with the disconnected client it says bad fd.
	* [client] nick accepts more than 1 argument
- [server-and-channel] when a channel has 0 clients -> remove from the server.
- [channel] channel name only # should not pass
- [client] should reciieve a message if not auhenticated
- [channel-name] should check if channel name is valid
-----------
- [message-format] it needs to end with \r\n

# TESTING
* for all channel commands we have to first check that the clliennt is authenticated
1. JOIN
	- channel name syntax 
	- Join a channel that already exists vs. a new one ✅
	- can join ✅
	- can't join
		* +i and not invited ✅
		* +k and wrong password ✅
		* +l and is full ✅
		* +l and is NOT full ✅
	- Try joining multiple channels at once (e.g. JOIN #a,#b,#c)
	- Try joining multiple channels at once (e.g. JOIN #a,#b,#c)

2. PRIVMSG
	- can send a message to chnnel ✅
	- can't send a message to channel ✅
	- can send a message to a client ✅
	- can't send a message to a client ✅
	- send to yourself

3. KICK
	- commander is moderator 
		* user exist ✅
		* user does not exist in channel ✅
		* user does not exist in server ✅
		* kick yourself ✅
	- commander is not moderator ✅
	- KICK/disconnect the last client ✅

4. INVITE
	- commander is moderator ✅
		* user does not exist ✅
		* user is not authenticated
	- commander is not moderator

5. TOPIC
	- get topic vs set topic ✅
	- commander is moderator ✅
		* syntax of the topic ✅
	- +t and commander is not moderator ✅

6. MODE
	- commander is moderator
		* +i ✅
		* -i ✅
		* +t ✅
		* -t ✅
		* +k [password-syntax] ✅
		* -k ✅
			* when key is not set ✅
		* +o 
			* user is not a channel member ✅
			* user is not a server member ✅
		* -o
			* user is not a channel member ✅
			* user is not a server member ✅
		* +l
			- what if users are already more than the limit? ✅
			- non-number limit ❌
		* invallid flag ✅
	- commander is not moderator ✅

| **Client Command**            | **Description**                 | **Expected Server Message(s)**                                                                               |
| ----------------------------- | ------------------------------- | ------------------------------------------------------------------------------------------------------------ |
| `JOIN #channel`               | Join a channel                  | `:<nick>!<user>@<host> JOIN :#channel`                                                                       |
|                               |                                 | `:server 332 <nick> #channel :<topic>` *(Topic)*                                                             |
|                               |                                 | `:server 353 <nick> = #channel :@admin +user1 user2` *(NAMES list)*                                          |
|                               |                                 | `:server 366 <nick> #channel :End of /NAMES list.`                                                           |
| `PRIVMSG <target> <message>`  | Send private or channel message | `:<nick>!<user>@<host> PRIVMSG <target> :<message>`                                                          |
| `KICK #channel nick [reason]` | Kick user from channel          | `:<nick>!<user>@<host> KICK #channel nick :[reason]`                                                         |
| `INVITE nick #channel`        | Invite user to a channel        | `:<nick>!<user>@<host> INVITE nick :#channel`                                                                |
| `TOPIC #channel :<topic>`     | Set or view channel topic       | **Set:** `:<nick>!<user>@<host> TOPIC #channel :<topic>`<br>**View:** `:server 332 <nick> #channel :<topic>` |
| `MODE #channel +k pass`       | Set password/key for channel    | `:<nick>!<user>@<host> MODE #channel +k pass`                                                                |
| `MODE #channel +i`            | Make channel invite-only        | `:<nick>!<user>@<host> MODE #channel +i`                                                                     |
| `MODE #channel +o nick`       | Give channel operator rights    | `:<nick>!<user>@<host> MODE #channel +o nick`                                                                |
| `MODE #channel +l limit`      | Set user limit                  | `:<nick>!<user>@<host> MODE #channel +l limit`                                                               |
| `MODE #channel +t`            | Only ops can set topic          | `:<nick>!<user>@<host> MODE #channel +t`                                                                     |





| **Code** | **Meaning**                                 | **Format (for LimeChat)**                                           |
| -------- | ------------------------------------------- | ------------------------------------------------------------------- |
| `401`    | ERR\_NOSUCHNICK – No such nick/channel      | `:server 401 <nick> <target> :No such nick/channel`                 |
| `403`    | ERR\_NOSUCHCHANNEL – No such channel        | `:server 403 <nick> #channel :No such channel`                      |
| `406`    | ERR\_WASNOSUCHNICK – Nickname not found     | `:server 406 <nick> <target> :There was no such nickname`           |
| `411`    | ERR\_NORECIPIENT – No recipient given       | `:server 411 <nick> :No recipient given (<command>)`                |
| `441`    | ERR\_USERNOTINCHANNEL – Not in that chan    | `:server 441 <nick> <target> #channel :They aren't on that channel` |
| `442`    | ERR\_NOTONCHANNEL – You're not on that chan | `:server 442 <nick> #channel :You're not on that channel`           |
| `443`    | ERR\_USERONCHANNEL – Already on channel     | `:server 443 <nick> <target> #channel :is already on channel`       |
| `461`    | ERR\_NEEDMOREPARAMS – Missing params        | `:server 461 <nick> <command> :Not enough parameters`               |
| `471`    | ERR\_CHANNELISFULL – Channel is full        | `:server 471 <nick> #channel :Cannot join channel (+l)`             |
| `472`    | ERR\_UNKNOWNMODE – Unknown mode             | `:server 472 <nick> <char> :is unknown mode char to me`             |
| `473`    | ERR\_INVITEONLYCHAN – Invite-only channel   | `:server 473 <nick> #channel :Cannot join channel (+i)`             |
| `475`    | ERR\_BADCHANNELKEY – Incorrect channel key  | `:server 475 <nick> #channel :Cannot join channel (+k)`             |
| `476`    | ERR\_BADCHANMASK – Bad channel name         | `:server 476 <nick> #badchan :Bad Channel Mask`                     |
| `482`    | ERR\_CHANOPRIVSNEEDED – You're not a chanop | `:server 482 <nick> #channel :You're not channel operator`          |
| `341`    | RPL\_INVITING – You invited someone         | `:server 341 <nick> <target> #channel`                              |
