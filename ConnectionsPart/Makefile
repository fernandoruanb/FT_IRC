# Name of the program or library

NAME = ircserver

# Compilers and flags

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98

#Directories

SRCDIR = .
INCDIR = .

# SRCS

SRCS =  internet.cpp\
	internet_utils/atoiIRC.cpp\
	internet_utils/getServer.cpp\
	internet_utils/manageBuffers.cpp\
	internet_srcs/serverIRCStartMode.cpp\
	internet_srcs/broadcast.cpp\
	internet_srcs/privmsg.cpp

#OBJS

OBJS = $(SRCS:.cpp=.o)

# Main

all: $(NAME)

# All the magic happens here

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(NAME) $(OBJS)

# .cpp -> .o

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#clean

clean:
	rm -f $(OBJS)

#full clean

fclean: clean
	rm -f $(NAME)

#restart

re: fclean all

#Run Mode

run: all
	@valgrind --track-origins=yes --show-leak-kinds=all --track-fds=yes --leak-check=full ./ircserver 6667 12345

.PHONY: all fclean clean re
