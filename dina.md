join, topic, spaces before the command
JOIN: 
~    - Join a new channel (channel creation) ✅
~    - Join an existing channel ✅
~    - Invalid channel name ✅
~    - Join a channel with correct key ✅
~    - Join invite-only channel with invitation ✅
~    - join multiple channels with keys ✅
~    - Join multiple channels ✅ 
~    - join a channel user is already in ✅
~    - JOIN 0 (leave all channels) ✅
~    - Join with too many keys ✅
~    - Join with wrong key ✅
~    - Join invite-only channel without invitation✅ 
~    - Join full channel ✅
~
~TOPIC:
~    -  Viewing topic when no topic is set ✅
~    -  Setting a new topic as channel operator ✅ the msh should be sent to the other clients in the chnl
~    -  Viewing topic when topic is set ✅
~    -  Clearing the topic ✅
~    -  Changing topic as non-operator in +t channel ✅
~    -  Missing channel name  ✅
~    -  Channel does not exist  ✅
~    -  Trying to set a topic without being on the channel  ✅
~
~INVITE:
~    - Operator invites a user to an invite-only channel ✅
~    - Operator invites a user to a public channel ✅
~    - Inviting a user already invited before joining ✅
~    - Non-operator tries to invite to invite-only channel ✅
~    - User is already in the channel ✅
~    - Inviting to a non-existent channel ✅
~    - Target nickname does not exist ✅
~    - Missing parameters (with no,one, ore two spaces after the command) (but it should work correctly in all cases- see next case)✅
~    - "/invite      " more than 2 spaces after the command ✅
~        output : No such nick
~        correct output : Not enough parameters
~    - "/invite existuser  " more than 1 space after the nick ❌
~        output : No such channel
~        correct output : Not enough parameters
~
~        
~MODE:
~    - View channel modes✅
~    - Set invite-only mode (+i)✅
~    - Set topic restriction mode (+t)✅
~    - Set a channel key (+k)✅
~    - Missing parameter for +k✅
~    - Remove a channel key (-k)✅
~    - Set a user limit (+l)✅ -------------------------- should handle limit value, negative + overflow
~    - Remove user limit (-l)✅ 
~    - Missing parameter for +l✅
~    - Give operator to another user (+o)✅
~    - Missing target nickname for +o✅
~    - Remove operator from user (-o)
~    - Not enough parameters (missing mode)✅
~    - Not a channel operator✅
~    - Channel does not exist✅
~    - Invalid mode character✅
~
~
~
~** to be handled
~/mode #c1 -ll
~:irc.server :dina!dina@127.0.0.1 MODE #c1 -ll
~
~- PRIVMSG :
~    extra spaces after command :No text to send, instead of  :No recipient given
~
~added by Sabira
~1. PASS
~    Test Case 1: Valid password
~    Input: PASS 123
~        Expected Output: No reply, client authenticated.✅
~        Explanation: handlePass sets isAuthenticated(true) (line 36). Silent per RFC 2812 (Section 3.1.1).
~    Test Case 2: Missing password
~    Input: PASS
~        Expected Output: :irc.server 461 nick :Not enough parameters✅
~        Explanation: tokens.size() < 2 (line 24) sends ERR_NEEDMOREPARAMS (461).
~    Test Case 3: Wrong password
~    Input: PASS wrong
~        Expected Output: :irc.server 464 * :Password incorrect, client disconnected.✅
~        Explanation: tokens[1] != _password (line 32) sends ERR_PASSWDMISMATCH (464), calls removeClient.
~    Test Case 4: Already authenticated
~    Input: PASS 123 (after PASS 123)
~        Expected Output: :irc.server 462 alice :Unauthorized command (already registered)✅
~        Explanation: isAuthenticated() true (line 28) sends ERR_ALREADYREGISTERED (462).
~    [NEW] Test Case 5: Partial password command
~    Input: PA^DSS 1^D23 (via nc -C, ^D = Ctrl+D)
~        Expected Output: No reply until full command, then authenticates.✅
~        Explanation: receiveClientData aggregates partial data (subject’s nc test), processes PASS 123 when complete.
~2. NICK
~    Test Case 1: Valid nickname
~    Input: NICK alice (only if!!!!!  PASS 123, USER alice 0 * :Alice Smith)
~
~        :irc.server 001 alice :Welcome to the IRC server
~        :irc.server 002 alice :Your host is irc.server
~        :irc.server 003 alice :This server was created today
~        :irc.server 004 alice :irc.server 1.0
~    Explanation: validateNick passes (line 45), registers client (line 61–66), sends RPL_WELCOME (001) etc. (lines 67–70).
~    Test Case 2: Missing nickname
~        Input: NICK
~        Expected Output: :irc.server 431 * :No nickname given✅
~        Explanation: tokens.size() < 2 (line 40) sends ERR_NONICKNAMEGIVEN (431).
~    Test Case 3: Invalid nickname
~        Input: NICK a@b
~        Expected Output: :irc.server 432 * a@b :Erroneous nickname✅
~        Explanation: validateNick fails (line 26–37), sends ERR_ERRONEUSNICKNAME (432).
~    Test Case 4: Nickname in use
~        Input: NICK alice (after another client sets alice)
~        Expected Output: :irc.server 433 * alice :Nickname is already in use✅
~        Explanation: Duplicate check (line 52–57) sends ERR_NICKNAMEINUSE (433).
~    [NEW] Test Case 5: Partial nickname command
~        Input: NI^DCK ali^Dce
~        Expected Output: No reply until full command, then registers alice.✅
~        Explanation: receiveClientData buffers partial data, processes NICK alice when complete.
~    Test Case 5: test with spaces
~        Input:     nick      test✅
~3. USER
~    Test Case 1: Valid user registration
~    Input: USER alice 0 * :Alice Smith (after PASS 123, NICK alice)✅
~    Expected Output:
~    text
~        :irc.server 001 alice :Welcome to the IRC server
~        :irc.server 002 alice :Your host is irc.server
~        :irc.server 003 alice :This server was created today
~        :irc.server 004 alice :irc.server 1.0
~    Explanation: validateUser passes (lines 4–37), registers client (line 115–120), sends RPL_WELCOME (lines 121–124).
~    
~    Test Case 2: Missing parameters
~        Input: USER alice
~        Expected Output: :irc.server 461 * USER :Not enough parameters✅
~        Explanation: tokens.size() < 5 (line 6) sends ERR_NEEDMOREPARAMS (461).
~    Test Case 3: Invalid username
~    Input: USER a@b 0 * :realname
~        Expected Output: :irc.server 461 * USER :Username contains invalid characters✅
~        Explanation: Username validation fails (line 15–20), sends ERR_NEEDMOREPARAMS.
~    Test Case 4: Already registered
~        Input: USER alice 0 * :realname (after registration)
~        Expected Output: :irc.server 462 alice :Unauthorized command (already registered) ✅
~        Explanation: isRegistered() true (line 104) sends ERR_ALREADYREGISTERED (462).
~4. PRIVMSG
~    Test Case 1: Valid channel message
~        Input: PRIVMSG #test :hello world (from alice, in #test with bob)✅
~        Expected Output: Bob receives :alice!alice@host PRIVMSG #test :hello world
~        Explanation: handlePrivmsg broadcasts to channel members (lines 182–185).
~    Test Case 2: No recipient
~        Input: PRIVMSG
~        Expected Output: :irc.server 411 alice PRIVMSG :No recipient given✅
~        Explanation: tokens.size() < 3 (line 164) sends ERR_NORECIPIENT (411).
~    Test Case 3: No text
~        Input: PRIVMSG #test :
~        Expected Output: :irc.server 412 alice PRIVMSG :No text to send✅
~        Explanation: message.empty() (line 178) sends ERR_NOTEXTTOSEND (412).
~    Test Case 4: Invalid target
~        Input: PRIVMSG #invalid hello
~        Expected Output: :irc.server 401 alice #invalid :No such nick/channel✅
~        Explanation: No channel invalid (line 187) sends ERR_NOSUCHNICK (401).
~    [NEW] Test Case 5: Multi-client channel message
~        Input: PRIVMSG #test :hello all (from alice, #test has bob, charlie)
~        Expected Output: Bob and Charlie receive :alice!alice@host PRIVMSG #test :hello all
~        Explanation: boolBroadCastToAll sends to all channel members, satisfying subject’s forwarding requirement.
~5. JOIN
~    Test Case 1: Join new channel ✅
~        Input: JOIN #test
~        Expected Output:
~        text
~        :alice!alice@host JOIN :#test
~        :irc.server 331 alice #test :No topic is set
~        :irc.server 353 alice = #test :@alice
~        :irc.server 366 alice #test :End of /NAMES list
~        Explanation: Creates channel (line 73), adds user/operator (lines 76–77), sends RPL_NOTOPIC (331), RPL_NAMREPLY (353), RPL_ENDOFNAMES (366) (lines 84–88).
~    Test Case 2: Join existing channel ✅
~        Input: JOIN #test (after alice joins)
~        Expected Output: Same as above, with :@alice @bob in RPL_NAMREPLY.
~        Explanation: Adds user to channel (line 98).
~    Test Case 3: Invalid channel ✅
~        Input: JOIN test
~        Expected Output: :irc.server 403 alice test :No such channel
~        Explanation: channelName[0] != '#' (line 66) sends ERR_NOSUCHCHANNEL (403).
~    Test Case 4: Join with key ✅
~        Input: JOIN #test secret (channel has key secret)
~        Expected Output: Success, same as Test Case 1.
~        Explanation: canJoin(key) passes (line 92).
~    [NEW] Test Case 5: Invite-only channel ✅
~        Input: JOIN #test (channel is +i, no invite)
~        Expected Output: :irc.server 473 alice #test :Cannot join channel (+i)
~        Explanation: isInviteOnly() && !isInvited (line 86) sends ERR_INVITEONLYCHAN (473).
~6. PART (Assumed Implementation)
~    - Test Case 1: Part channel (existing channel you’re in) ✅
~        Input: PART #test
~        Expected Output: :alice!alice@host PART #test, client leaves channel.
~        Explanation: Broadcasts PART, removes user (RFC 2812, Section 3.2.2).
~    - Test Case 2: Part with reason (existing channel you’re in)  ✅
~        Input: PART #test :bye
~        Expected Output: :alice!alice@host PART #test :bye
~        Explanation: Includes reason in broadcast.
~    - Test Case 3: Invalid channel ✅
~        Input: PART #invalid
~        Expected Output: :irc.server 403 alice #invalid :No such channel
~        Explanation: Sends ERR_NOSUCHCHANNEL (403).
~    - Test Case 4: Part all channels
~        Input: JOIN 0
~        Expected Output: :alice!alice@host PART #test, leaves all channels.
~        Explanation: channelName == "0" (line 60) parts all channels (line 63–71).
~    - PART multiple channels you're in ✅
~        Input: PART #general,#test
~        Expected Output:
~        alice!alice@host PART #general
~        alice!alice@host PART #test
~    - PART multiple channels with a reason ✅
~        Input: PART #general,#test :reason
~        Expected Output:
~        alice!alice@host PART #general :reason
~        alice!alice@host PART #test :reason
~    - PART a channel you're not in ✅
~        Input: PART #notjoined
~        Expected Output: :irc.server 403 alice #notjoined :You're not on that channel
~    - PART with missing channel name ✅
~        Input: PART 
~        Expected Output: :irc.server 461 alice PART :Not enough parameters
~    - PART with malformed channel name ✅
~        Input: PART general
~        Expected Output: :irc.server 403 alice general :No such channel
~    -  PART with extra commas ✅
~        Input: PART #general,,
~        Expected Output: Server Might Respond (depending on implementation):
~                :irc.server 403 alice  :No such channel
~                :irc.server 442 alice #general :You're not on that channel
~    - PART with extra spaces after command ✅
~        Input: PART        #general
~        Expected Output: LimeChat will still parse it correctly
~                :alice!alice@host PART #general
~    - PART after leaving the channel already ✅
~        Input: PART #general
~        Client Input again: PART #general
~        Expected Output: :irc.server 442 alice #general :You're not on that channel //if channel exists
~        Expected Output: :irc.server 442 alice #general :No such channel //if channel does not exist
~.
~7. TOPIC
~    Test Case 1: Set topic ✅
~    Input: TOPIC #test :Hello world
~    Expected Output: :alice!alice@host TOPIC #test :Hello world (to channel).
~    Explanation: Sets topic (line 156), broadcasts (line 160).
~    Test Case 2: Get topic ✅
~    Input: TOPIC #test
~    Expected Output: :irc.server 332 alice #test :Hello world or :irc.server 331 alice #test :No topic is set
~    Explanation: Sends RPL_TOPIC (332) or RPL_NOTOPIC (331, lines 148–151).
~    Test Case 3: Invalid channel ✅
~    Input: TOPIC test
~    Expected Output: :irc.server 403 alice test :No such channel
~    Explanation: channelName[0] != '#' (line 134) sends ERR_NOSUCHCHANNEL (403).
~    Test Case 4: Non-operator in restricted channel ✅
~    Input: TOPIC #test :new topic (non-operator, +t)
~    Expected Output: :irc.server 482 alice #test :You're not channel operator
~    Explanation: isTopicRestricted() && !isOperator (line 152) sends ERR_CHANOPRIVSNEEDED (482).
~8. INVITE
~    Test Case 1: Valid invite ✅
~        Input: INVITE bob #test
~        Expected Output:
~        Sender: :irc.server 341 alice bob #test
~        Invitee: :alice!alice@host INVITE bob :#test
~        Explanation: Adds invite (line 180), sends RPL_INVITING (341) and invite message (lines 182–185).
~    Test Case 2: Missing parameters ✅
~        Input: INVITE bob
~        Expected Output: :irc.server 461 alice INVITE :Not enough parameters
~        Explanation: tokens.size() < 3 (line 164) sends ERR_NEEDMOREPARAMS (461).
~    Test Case 3: Invalid nick ✅
~        Input: INVITE invalid #test
~        Expected Output: :irc.server 401 alice invalid :No such nick
~        Explanation: getClientByNickname fails (line 169) sends ERR_NOSUCHNICK (401).
~     Test Case 4: User already on channel ✅
~        Input: INVITE bob #test (bob in channel)
~        Expected Output: :irc.server 443 alice bob #test :is already on channel
~        Explanation: isUser(targetFd) (line 178) sends ERR_USERONCHANNEL (443).
~9. KICK
~    Test Case 1: Valid kick
~    Input: KICK #test bob :rude
~        Expected Output: :alice!alice@host KICK #test bob :rude, bob removed.
~        Explanation: Broadcasts (line 217), removes user (line 218).
~    Test Case 2: Missing parameters
~    Input: KICK #test
~        Expected Output: :irc.server 461 alice KICK :Not enough parameters
~        Explanation: tokens.size() < 3 (line 192) sends ERR_NEEDMOREPARAMS (461).
~    Test Case 3: Non-operator
~    Input: KICK #test bob (non-operator)
~        Expected Output: :irc.server 482 alice #test :You're not channel operator
~        Explanation: !isOperator (line 203) sends ERR_CHANOPRIVSNEEDED (482).
~    Test Case 4: Target not in channel
~    Input: KICK #test bob
~        Expected Output: :irc.server 441 alice bob #test :They aren't on that channel
~        Explanation: !isUser(targetFd) (line 213) sends ERR_USERNOTINCHANNEL (441).
~    - Kicking a user with a reason
~    Input: KICK #test bob :being disruptive 
~        Expected Output: :alice!alice@host KICK #test bob :being disruptive
~        Explanation: alice kicks bob from the channel with a custom reason that will be broadcast to the remaining users in the channel
~    - Kicking from a channel that doesn't exist
~    Input: KICK #doesnotexist bob
~        Expected Output: :irc.server 403 alice #doesnotexist :No such channel
~        Explanation: The channel doesn't exist. The server responds with ERR_NOSUCHCHANNEL (403).
~    - Kicking a user while not on the channel yourself
~    Input: KICK #test bob
~        Expected Output: :irc.server 442 alice #test :You're not on that channel
~        Explanation: alice is trying to kick someone from a channel she is not part of. Server replies with ERR_NOTONCHANNEL (442).
~    - Kicking multiple users from one channel
~    Input: KICK #test bob,charlie :cleanup
~        Expected Output: :alice!alice@host KICK #test bob :cleanup  
~                         :alice!alice@host KICK #test charlie :cleanup
~
~        Explanation: Multiple users are kicked from the same channel in a single command. A message is sent for each user kicked.
~    - Kicking from multiple channels (one user per channel)
~    Input: KICK #chan1,#chan2 bob,charlie
~
~        Expected Output: :alice!alice@host KICK #chan1 bob :alice  
~                         :alice!alice@host KICK #chan2 charlie :alice
~
~        Explanation: The command kicks bob from #chan1 and charlie from #chan2. The server processes parallel lists of channels and users.
~    - Invalid syntax — too few parameters
~    Input: KICK  or  "           kick"   or  "kick              "
~        Expected Output: :irc.server 461 alice KICK :Not enough parameters
~        Explanation: Missing channel and user. The server returns ERR_NEEDMOREPARAMS (461).
~    - Too many users or channels — mismatched list sizes
~    Input: KICK #chan1,#chan2 bob
~        Expected Output: :irc.server 461 alice KICK :Mismatched parameters
~        Explanation: Number of channels and users don't match unless only one user is provided for all channels. If not allowed by your spec, return an error.
~
~10. MODE
~    Test Case 1: Query channel modes
~    Input: MODE #test
~        Expected Output: :irc.server 324 alice #test +nt
~        Explanation: tokens.size() == 2 (line 233) sends RPL_CHANNELMODEIS (324).
~    Test Case 2: Set invite-only
~    Input: MODE #test +i
~        Expected Output: :alice!alice@host MODE #test +i (to channel).
~        Explanation: setInviteFlag (line 249), broadcasts (line 286).
~    Test Case 3: Set key
~    Input: MODE #test +k secret
~        Expected Output: :alice!alice@host MODE #test +k secret
~        Explanation: setKeyMode (line 258).
~    Test Case 4: Invalid mode
~    Input: MODE #test +x
~        Expected Output: :irc.server 472 alice x :is unknown mode char
~        Explanation: Unknown mode (line 280) sends ERR_UNKNOWNMODE (472).
~    [NEW] Test Case 5: Remove key
~    Input: MODE #test -k
~        Expected Output: :alice!alice@host MODE #test -k
~        Explanation: setKeyMode('-') (line 258) removes key.
~    [NEW] Test Case 6: Invalid user limit
~    Input: MODE #test +l abc
~        Expected Output: :irc.server 461 alice MODE :Invalid limit
~        Explanation: stringToInt fails (line 275), sends ERR_NEEDMOREPARAMS.
~11. QUIT
~    Test Case 1: Quit with message
~    Input: QUIT :bye
~        Expected Output:
~        Channel members receive: :alice!alice@host QUIT :bye
~        Client disconnected.
~        Explanation: handleQuit broadcasts to channels (lines 14–18), removes client (line 20).
~    Test Case 2: Quit without message
~    Input: QUIT
~        Expected Output:
~        Channel members receive: :alice!alice@host QUIT :Client quit
~        Client disconnected.
~        Explanation: Default message (line 5).
~    Test Case 3: No channels
~    Input: QUIT
~        Expected Output: Client disconnected, no broadcast.
~        Explanation: No channels skips broadcast.
~    [NEW] Test Case 4: Quit in multiple channels
~    Input: QUIT :bye (alice in #test, #dev)
~        Expected Output: All members in #test, #dev receive :alice!alice@host QUIT :bye
~        Explanation: boolBroadCastToAll notifies all channels (lines 14–18).
~12. Non-blocking I/O
~    [NEW] Test Case 1: Multiple clients
~    Input: Connect 5 clients (netcat/LimeChat), send PRIVMSG #test :hi from each.
~        Expected Output: All clients in #test receive messages without delay.
~        Explanation: select in runServer handles multiple FDs non-blocking (subject requirement).
~    [NEW] Test Case 2: High load
~    Input: 10 clients send rapid PING server commands.
~        Expected Output: All receive :irc.server PONG :server without hangs.
~        Explanation: Non-blocking I/O ensures no client blocks others.
~    
~    Testing:
~    Netcat: printf "COMMAND\r\n" | nc -C 127.0.0.1 6667
~    LimeChat: Test channel operations, operator commands.
~    Valgrind: valgrind ./ircserv 6667 123 for leaks.