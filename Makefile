CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFILES = Server.cpp Client.cpp Channel.cpp main.cpp
NAME = ircserv

all: $(NAME)

$(NAME):
	$(CPP) $(CPPFLAGS) $(CPPFILES) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean:
	rm -rf $(NAME)

re: clean all

