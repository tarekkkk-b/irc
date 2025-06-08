NAME= ircserve

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 -g3

# Colors and formatting
BLUE := \033[1;34m
GREEN := \033[1;32m
YELLOW := \033[1;33m
RED := \033[1;31m
RESET := \033[0m
CLEAR := \033[2K
CURSOR_UP := \033[1A

# Detect OS for kqueue handling
OS := $(shell uname -s)
ifeq ($(OS), Linux)
	# Check if libkqueue is installed
	IFKQUEUE := $(shell if [ -f /usr/local/include/kqueue/sys/event.h ]; then echo "found"; else echo "missing"; fi)
	CXXFLAGS += -I/usr/local/include
	LDFLAGS = -L/usr/local/lib -lkqueue
endif

SRC= main.cpp \
	 srcs/Server.cpp \
	 srcs/Channel.cpp \
	 srcs/Client.cpp 
	 

# Progress bar variables
COUNTER := 0
BAR_LENGTH := 40
TOTAL_FILES := $(words $(SRC))

define update_progress
    -@$(eval COUNTER=$(shell echo $$(($(COUNTER) + 1))))
    -@$(eval PERCENTAGE=$(shell echo $$(($(COUNTER) * 100 / $(TOTAL_FILES)))))
    -@$(eval FILLED=$(shell echo $$(($(COUNTER) * $(BAR_LENGTH) / $(TOTAL_FILES)))))
    -@$(eval EMPTY=$(shell echo $$(($(BAR_LENGTH) - $(FILLED)))))
    -@printf "$(CLEAR)$(CURSOR_UP)"
    -@printf "$(YELLOW)Building [$(RESET)"
    -@printf "%-*s" $(FILLED) | tr ' ' '█'
    -@printf "%-*s" $(EMPTY) | tr ' ' '.'
    -@printf "$(YELLOW)] %3d%% - %s$(RESET)\n" $(PERCENTAGE) "$<"
endef

OBJ= $(SRC:.cpp=.o)

%.o: %.cpp
	@if [ $(COUNTER) = 0 ]; then printf "\n"; fi
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -g3
	-@$(call update_progress)

# Individual dependencies
run: all
	@echo "$(BLUE)Running the IRC server...$(RESET)"
	@./$(NAME) 6667 123

$(NAME): $(OBJ)
	@echo "\n$(GREEN)Linking object files...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)✓ Successfully built $(NAME)$(RESET)\n"

clean:
	@echo "$(RED)Cleaning up object files...$(RESET)"
	@rm -f $(OBJ)
	@rm -f IRC_tester

fclean: clean
	@echo "$(RED)Cleaning up executable and object files...$(RESET)"
	@rm -f $(NAME)
	@- make rmirssi1
	@- make rmirssi2

re: fclean all
	@echo "$(GREEN)Rebuilding complete!$(RESET)"

nc:
	@echo "$(YELLOW)Connecting to IRC server...$(RESET)"
	@nc 127.0.0.1 6667 

irssi1:
	@echo "$(GREEN)Starting IRSSI client 1...$(RESET)"
	@-docker run -it --name irssi-sender -e TERM -u $(shell id -u):$(shell id -g) \
	--log-driver=none \
	-v ${HOME}/.irssi-sender:/home/user/.irssi \
	irssi

irssi2:
	@echo "$(GREEN)Starting IRSSI client 2...$(RESET)"
	@-docker run -it --name irssi-receiver -e TERM -u $(shell id -u):$(shell id -g) \
	--log-driver=none \
	-v ${HOME}/.irssi-receiver:/home/user/.irssi \
	irssi

rmirssi1:
	@echo "$(RED)Removing IRSSI client 1...$(RESET)"
	@-docker rm -f irssi-sender 2>/dev/null

rmirssi2:
	@echo "$(RED)Removing IRSSI client 2...$(RESET)"
	@-docker rm -f irssi-receiver 2>/dev/null

test:
	@echo "$(BLUE)Running IRC server tests...$(RESET)"
	@c++ -o IRC_tester IRC_tester.cpp
	@./IRC_tester

# Install libkqueue if needed (Linux only)
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
endif

# Make all depend on install-deps
all: install-deps $(NAME)

.PHONY: all clean fclean re run nc install-deps test irssi1 irssi2 rmirssi1 rmirssi2 