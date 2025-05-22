CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFILES = srcs/Server.cpp main.cpp srcs/Client.cpp srcs/Channel.cpp 
NAME = ircserv

all: $(NAME)

$(NAME): $(CPPFILES)
	$(CPP) $(CPPFLAGS) $(CPPFILES) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean:
	rm -rf $(NAME)

re: clean all