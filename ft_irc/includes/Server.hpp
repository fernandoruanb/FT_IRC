/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 13:34:33 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/18 16:08:13 by jopereir         ###   ########.fr       */
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
# include <limits.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include "colours.hpp"

#define SERVER_NAME "irc.maroto.com"

class Channel;

/*
	line	- the full input buffer
		ex-USER Miku irc ft_irc
	args	- a substr of line containing only the arguments
		ex-Miku irc ft_irc
	index	- the poll index
	fd 		- Client fd
	argc	- args[] size
*/
struct	s_commands
{
	std::string				&line;
	std::map<int, Client*>* &clients;
	int						fd;
	int						index;
	std::string				*args;
	size_t					argc;

	s_commands(std::string &l, std::map<int, Client*>* &c, int f, int i, std::string &a)
        : line(l), clients(c), fd(f), index(i)
	{
		if (a.empty())
		{
			argc = 0;
			args = NULL;
			return;
		}
		size_t	j = 0;

		for (size_t i = 0; i < a.size(); i++)
			if (a[i] == ' ')
				j++;
		
		args = new std::string[j + 1];
		argc = j + 1;

		size_t	tokenI = 0;
		size_t	start = 0;
		for (size_t i = 0; i < a.size(); i++)
			if (a[i] == ' ')
			{
				args[tokenI++] = a.substr(start, i - start);
				start = i + 1;
			}
		
		args[tokenI] = a.substr(start, a.size() - start);
	}

	~s_commands(void)
	{
		if (argc)
			delete[] args;
	}
};

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
		void    inviteToChannel(std::string channelName, int operatorFD, int clientFD);
		bool	checkName(std::string Name);
		void	changeChannelInviteFlag(std::string channel, bool flag);
		int	getChannelsIndex(std::string channel);
		int	getClientsIndex(int clientFD);
		void    promotionChannelOperator(std::string channel, int owner, int clientFD);
		void	changeChannel(std::string Name, int clientFD);
		void	deleteChannel(std::string Name, int clientFD);
		void    removeOperatorPrivilegesFromEveryBody(std::string channel);
		void    createNewChannel(std::string Name, int clientFD);
		void    kickFromChannel(std::string channel, int owner, int clientFD);
		void	addNewClient(int clientFD);
		void    changeTopic(std::string channelName, int clientFD, std::string topic);
		void	startIRCService(void);
		void	manageBuffers(int index);
		void	shutdownService(void);
		void	broadcast(int index);
		void	chargePrivileges(int target);
		void	startPollFds(void);
		bool	handleClientAuthentication(std::map<int, Client*>* clients, int fd, char* buffer, int pollIndex);
		void	handlePing(s_commands &commands);
		void	privmsg(int index, int sender, std::string message);
		void	init(int port, std::string password);
		int	findGoodIndex(void);
		int	atoiIRC(std::string port);
		void	PollServerRoom(void);
		void	PollInputClientMonitoring(void);
		void	PollOutMonitoring(void);
		static void	handleSignal(int signal);
		bool	isValidArgs(const std::string &buffer, size_t pos, bool &op);
		std::string	getText(std::string& buffer, size_t *pos, std::map<int, Client*>* clients, bool check_name);
		void	user(s_commands	&commands);
		bool	handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i);
		void	mode(s_commands &com);
		void	nick(s_commands&);
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
		int	getNumberOfClients(void) const;
		struct pollfd	(&getPollFds(void))[1024];
};
std::ofstream operator<<(std::ostream &out, const Server &other);

bool	isEmptyInput(const std::string &line);
std::map<int, Channel*>* getChannelsMap(void);
std::map<int, Client*>* getClientsMap(void);
struct pollfd(*getMyFds(void))[1024];
bool	*getRunning(void);

#endif /* SERVER_HPP */
