/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internet.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 15:04:30 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/08 14:25:00 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERNET_HPP
# define INTERNET_HPP

# include <poll.h>
# include <iostream>
# include <signal.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <errno.h>
# include "colours.hpp"
# include "Client.hpp"

class Client;

typedef struct	s_server
{
	int	serverIRC;
	sockaddr_in	server;
	sockaddr_in	client;
	socklen_t	client_len;
	Client		myClients[1024];
	struct pollfd	fds[1024];
	std::string	sendBuffer[1023];
	std::string	recvBuffer[1024];
	nfds_t	nclFD;
	int	clFD;
	int	opt;
	bool	running;
	int	timeout;
	std::string	clientMessage;
	std::string	serverMessage;
	char	buffer[513];
	ssize_t	bytes;
}	t_server;

int	atoiIRC(const char *port, int *err);
t_server*	getServer(void);
void	serverIRCStartMode(void);
void	broadcast(const int sender);
void	privmsg(const int target, const std::string message);
void	manageBuffers(int index);

#endif /* INTERNET_HPP */
