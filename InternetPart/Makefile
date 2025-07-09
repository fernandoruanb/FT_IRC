#Name of the program or the library

NAME = ircserver

#Compilers and flags

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98

#Directories

SRCDIR = .
INCDIR = .

# SRCS

SRCS =  Server.cpp \
	main.cpp

# OBJS

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
	rm -rf $(OBJS)

# full clear

fclean: clean
	rm -rf $(NAME)

run: all
	@valgrind --track-origins=yes --show-leak-kinds=all --track-fds=yes --leak-check=full ./ircserver 6667 12345

# restart

re: fclean all

.PHONY: all fclean clean re
