#Name of the program or the library

NAME = ircserver

#Compilers and flags

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -g3

#Directories

SRC_DIR = src/
INC_DIR = includes
BIN_DIR = ./bin/

# SRCS

SRCS =	utils/messages.cpp \
		server/Server.cpp \
		server/Server_poll.cpp \
		server/Server_utils.cpp \
		server/commands/Server_user.cpp \
		server/commands/Server_ping.cpp \
		server/Server_broadcast.cpp \
		server/Server_channel.cpp \
		server/Server_getters.cpp \
		server/Server_setters.cpp \
		Client.cpp \
		Channel.cpp \
		main.cpp

# OBJS

OBJS = $(addprefix $(BIN_DIR), $(SRCS:.cpp=.o))
# OBJS = $(SRCS:.cpp=.o)

# Colors
BLUE = "\033[34;1m"
GREEN = "\033[32;1m"
RED = "\033[31;1m"
CYAN = "\033[36;1;3;208m"
WHITE = "\033[37;1;4m"
END = "\033[0m"

# Main

all: $(BIN_DIR) $(NAME)
# all: $(NAME)

# All the magic happens here

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -I $(INC_DIR) -o $(NAME) $(OBJS)
	@echo $(GREEN)"-----------------------------------------"$(END)
	@echo $(GREEN)"|             IRC COMPILED              |"$(END)
	@echo $(GREEN)"-----------------------------------------"$(END)

# .cpp -> .o

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(BIN_DIR)%.o: $(SRC_DIR)%.cpp | $(BIN_DIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@
	@echo $(CYAN)"Compiling $< to $@"$(END)

# %.o:%.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@

#clean

clean:
	@rm -rf $(OBJS)
	@rm -rf $(BIN_DIR)
	@echo $(RED)"Objects removed"$(END)

# full clear

fclean: clean
	@rm -rf $(NAME)
	@echo $(RED)"Executable removed"$(END)

run: re
	@valgrind --track-origins=yes --show-leak-kinds=all --track-fds=yes --leak-check=full ./ircserver 6667 12345

# restart

re: fclean all

.PHONY: all fclean clean re
