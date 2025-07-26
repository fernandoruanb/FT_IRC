/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcaldas- <fcaldas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 13:34:33 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/24 17:10:30 by fcaldas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include "Channel.hpp"
# include "Client.hpp"
# include <iostream>
# include <cstdlib>
# include <fcntl.h>
# include <signal.h>
# include <poll.h>
# include <errno.h>
# include <limits.h>
# include <vector>
# include <ctime>
# include <sstream>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <algorithm>
# include "colours.hpp"
# include "messages.hpp"

# define SERVER_NAME "irc.maroto.com"
# define VERSION "1.0"

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
	std::string					&sendBuffer;
	std::string					&line;
	std::map<int, Client*>* 	&clients;
	Client*						client;
	int							fd;
	int							index;
	std::vector<std::string>	args;
	std::string					command;

	s_commands(std::string &l, std::map<int, Client*>* &c, int f, int i, std::string &a, std::string& com, std::string &buf);
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

		int     getClientsFdByName(std::string nickname);
		void    inviteToChannel(std::string channelName, int operatorFD, int clientFD);
		bool	checkName(std::string Name);
		void	changeChannelInviteFlag(std::string channel, bool flag);
		int	getChannelsIndex(std::string channel);
		bool    AuthenticationKeyProcess(const std::string channel, const std::string key);
		int	getClientsIndex(int clientFD);
		void    promotionChannelOperator(std::string channel, int owner, int clientFD);
		void	changeChannel(std::string Name, int clientFD, bool flag);
		void	deleteChannel(std::string Name, int clientFD);
		void    removeOperatorPrivilegesFromEveryBody(std::string channel);
		void    createNewChannel(std::string Name, int clientFD);
		void    kickFromChannel(std::string channel, int owner, int clientFD);
		void	addNewClient(int clientFD);
		void    changeTopic(std::string channelName, int clientFD, std::string topic);
		void	startIRCService(void);
		void	manageBuffers(int index);
		void	shutdownService(void);
		void	broadcast(int index, std::string line, int targetChannel = -1);
		void	chargePrivileges(int target);
		void	startPollFds(void);
		bool	handleClientAuthentication(std::map<int, Client*>* clients, int fd, char* buffer, int pollIndex);
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
		void	tryRegister(s_commands& com);
		
		//Commands
		void	user(s_commands&);
		bool	handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i);
		void	mode(s_commands&);
		void	nick(s_commands&);
		void	handlePing(s_commands &);
		void	invite(s_commands&);
		void	join(s_commands&);
		void	kick(s_commands&);
		void	kill(s_commands&);
		void	list(s_commands&);
		void	motd(s_commands&);
		void	names(s_commands&);
		void	notice(s_commands&);
		void	oper(s_commands&);
		void	part(s_commands&);
		void	pass(s_commands&);
		void	privmsg(s_commands&);
		void	quit(s_commands&);
		void	topic(s_commands&);
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
bool	findMode(const std::string& myModes, const char mode);
bool	validNick(s_commands& com);

#endif /* SERVER_HPP */
