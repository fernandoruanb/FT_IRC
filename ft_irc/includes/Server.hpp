/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 13:34:33 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/18 17:38:35 by jopereir         ###   ########.fr       */
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
# include <vector>
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
	args	- a vector of commands arguments
		ex-Miku irc ft_irc
	index	- the poll index
	fd 		- Client fd
*/
struct	s_commands
{
	std::string					&line;
	std::map<int, Client*>* 	&clients;
	Client*						client;
	int							fd;
	int							index;
	std::vector<std::string>	args;

	s_commands(std::string &l, std::map<int, Client*>* &c, int f, int i, std::string &a)
        : line(l), clients(c), fd(f), index(i)
	{
		client = NULL;
		std::map<int, Client*>::iterator it = clients->find(fd);
		if (it != clients->end())
			client = it->second;
		
		if (a.empty() || a[0] == '\n' || a[0] == '\r')
			return;
		size_t	start = 0;
		size_t	j = 0;
		for (j = 0; j < a.size(); j++)
			if (a[j] == ' ')
			{
				args.push_back(a.substr(start, j - start));
				start = j + 1;
			}
		if (start < a.size())
		{
			size_t	k = 0;
			while (a[k] && a[k] != '\n' && a[k] != '\r')
				k++;
			args.push_back(a.substr(start, k));
		}
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
