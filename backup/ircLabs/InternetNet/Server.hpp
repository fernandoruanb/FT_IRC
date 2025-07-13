/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nasser <nasser@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 13:34:33 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/11 14:14:58 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
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

#define SERVER_NAME "irc.maroto.com"

class Channel;

class	Server
{
	private:
		Server(void);
		struct pollfd (*fds)[1024];
		std::string	recvBuffer[1024];
		std::string	sendBuffer[1024];
		std::map<int, Channel*>* channels;
		std::map<int, Client*>* clients;
		int	serverIRC;
		int	numClients;
		int	numChannels;
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
		bool	checkName(std::string Name);
		void	changeChannel(std::string Name, int clientFD);
		void	deleteChannel(std::string Name, int clientFD);
		void    createNewChannel(std::string Name);
		void	startIRCService(void);
		void	shutdownService(void);
		void	addNewClient(int clientFD);
		bool	handleClientAuthentication(std::map<int, Client*>* clients, int fd, char* buffer, int pollIndex);
		void	startPollFds(void);
		void	manageBuffers(int index);
		void	broadcast(int index);
		void	privmsg(int index, int sender, std::string message);
		void	chargePrivileges(int target);
		void	PollServerRoom(void);
		void	PollInputClientMonitoring(void);
		void	PollOutMonitoring(void);
		static void	handleSignal(int signal);
};
std::ofstream operator<<(std::ostream &out, const Server &other);

#endif /* SERVER_HPP */
