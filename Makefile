CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
COMMANDS = "srcs/commands"
CPPFILES = srcs/Server.cpp main.cpp srcs/Client.cpp srcs/Channel.cpp srcs/redirect.cpp
CPPFILES += $(COMMANDS)/inivte.cpp $(COMMANDS)/join.cpp $(COMMANDS)/kick.cpp \
			$(COMMANDS)/mode.cpp $(COMMANDS)/privmsg.cpp $(COMMANDS)/topic.cpp
NAME = ircserv


all: $(NAME)

$(NAME): $(CPPFILES)
	$(CPP) $(CPPFLAGS) $(CPPFILES) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean:
	rm -rf $(NAME)

re: clean all