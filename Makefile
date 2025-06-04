CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFILES = srcs/Server.cpp main.cpp srcs/Client.cpp srcs/Channel.cpp 
NAME = ircserv


# Detect OS for kqueue handling
OS := $(shell uname -s)
ifeq ($(OS), Linux)
	# Check if libkqueue is installed
	IFKQUEUE := $(shell if [ -f /usr/local/include/kqueue/sys/event.h ]; then echo "found"; else echo "missing"; fi)
	CXXFLAGS += -I/usr/local/include
	LDFLAGS = -L/usr/local/lib -lkqueue
endif

all: $(NAME)

$(NAME): $(CPPFILES)
	$(CPP) $(CPPFLAGS) $(CPPFILES) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean:
	rm -rf $(NAME)

re: clean all