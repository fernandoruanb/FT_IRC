/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 13:34:33 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/10 11:57:44 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"
# include "Client.hpp"
# include <iostream>
# include <fcntl.h>
# include <signal.h>
# include <poll.h>
# include <errno.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include "colours.hpp"
# include <vector>
# include "Client.hpp"

class	Server
{
	private:
		Server(void);
		Client	myClients[1024];
		struct pollfd (*fds)[1024];
		std::string	recvBuffer[1024];
		std::string	sendBuffer[1024];
		int	serverIRC;
		int	numClients;
		int	port;
		std::string	password;
		bool	*running;
		Server&	operator=(const Server &other);
		Server(const Server &other);
	public:
		Server(std::string portCheck, std::string password);
		~Server(void);
		void	setPassword(std::string password);
		void	setPort(int Port);
		void	setIsRunning(bool signal);
		void	setServerIRC(int serverFD);
		int	getServerIRC(void) const;
		std::string	getPassword(void) const;
		int	getPort(void)	const;
		int	atoiIRC(std::string port);
		void	init(int port, std::string password);
		int	getNumberOfClients(void) const;
		struct pollfd	(&getPollFds(void))[1024];
		void	startIRCService(void);
		void	shutdownService(void);
		void	addNewClient(int clientFD);
		void	startPollFds(void);
		void	manageBuffers(int index);
		void	broadcast(int index);
		void	privmsg(int index, std::string message);
		void	chargePrivileges(int target);
		void	PollServerRoom(void);
		void	PollInputClientMonitoring(void);
		void	PollOutMonitoring(void);
		static void	handleSignal(int signal);
};
std::ofstream operator<<(std::ostream &out, const Server &other);

#endif /* SERVER_HPP */
