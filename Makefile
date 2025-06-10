# CPP = c+
CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinc 
COMMANDS = srcs/commands
CPPFILES = srcs/Server.cpp main.cpp srcs/Client.cpp srcs/Channel.cpp srcs/redirect.cpp \
			$(COMMANDS)/invite.cpp $(COMMANDS)/join.cpp $(COMMANDS)/kick.cpp \
			$(COMMANDS)/mode.cpp $(COMMANDS)/privmsg.cpp $(COMMANDS)/topic.cpp
NAME = ircserv

# Color codes
YELLOW = \033[1;33m
GREEN = \033[1;32m
RESET = \033[0m

# Initialize LDFLAGS (empty by default)
LDFLAGS = 

OS := $(shell uname -s)
ifeq ($(OS), Linux)
	# Check if libkqueue is installed
	IFKQUEUE := $(shell if [ -f /usr/local/include/kqueue/sys/event.h ]; then echo "found"; else echo "missing"; fi)
	CPPFLAGS += -I/usr/local/include
	LDFLAGS = -L/usr/local/lib -lkqueue
endif

all: install-deps $(NAME)

install-deps:
ifeq ($(OS), Linux)
ifeq ($(IFKQUEUE), missing)
	@echo "$(YELLOW)Installing libkqueue from GitHub...$(RESET)"
	-@apt update && apt install -y g++ gcc make cmake git 
	@if [ ! -d libkqueue ]; then git clone https://github.com/mheily/libkqueue.git; fi
	-@cd libkqueue && mkdir -p build && cd build && cmake .. && make
	@echo "$(YELLOW)Installing libkqueue to system...$(RESET)"
	-@cd libkqueue/build && make install
	-@ldconfig
	@echo "$(GREEN)libkqueue installed successfully$(RESET)"
else
	@echo "$(GREEN)libkqueue is already installed$(RESET)"
endif
else
	@echo "$(GREEN)Using native kqueue (macOS/BSD)$(RESET)"
endif


$(NAME): $(CPPFILES)
	$(CPP) $(CPPFLAGS) $(CPPFILES) -o $(NAME) $(LDFLAGS)

clean:
	rm -rf $(NAME)

fclean: clean
	rm -rf libkqueue

re: clean all

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=kqueue.supp ./ircserv 6667 XYZ 2> test

.PHONY: all clean fclean re install-deps valgrind