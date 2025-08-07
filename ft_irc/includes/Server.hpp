/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nasser <nasser@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 13:34:33 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/08/07 11:26:01 by fruan-ba         ###   ########.fr       */
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
	bool		isOnline;

	s_commands(std::string &l, std::map<int, Client*>* &c, int f, int i, std::string &a, std::string& com, std::string &buf);
};

struct	s_mode
{
	char	sign;
	char	flag;
	bool	flagFound;
	Channel* &target;
	std::string	&currentMode;
	size_t		len;
	id_t		channelIndex;
	bool		isKing;

	s_mode(char s, char f, bool ff, Channel* &t, std::string& c, size_t l, int i, bool k);
};

class	Server
{
	private:
		Server(void);
		struct pollfd (*fds)[1024];
		s_commands	*currentCommand;
		std::string	recvBuffer[1024];
		std::string	sendBuffer[1024];
		std::map<int, Channel*>* channels;
		std::map<int, Client*>* clients;
		std::set<int> kingsOfIRC;
		std::string	supremeKey;
		std::string	supremeUser;
		int	serverIRC;
		int	numClients;
		int	numChannels;
		int	port;
		std::string	password;
		bool	*running;
		Server&	operator=(const Server &other);
		Server(const Server &other);

		void    newBroadcastAllChannels(int clientFD, std::string message, std::string channel, bool flag);
		void    newBroadcastKill(s_commands& com, std::string msg, std::string complement, std::string channelName, bool flag);
		void    newBroadcast(s_commands& com, std::string msg, std::string channelName, bool flag);
		bool    checkCompatibility(int ownerFD, int clientFD, std::string targetChannel);
		void    removeAllChannelsOfClient(int clientFD);
		int	getClientFDByNick(std::string nickname, int numClients);
		int     getClientsFdByName(std::string nickname);
		void    inviteToChannel(std::string channelName, int operatorFD, int clientFD);
		bool	checkName(std::string Name);
		void	changeChannelInviteFlag(std::string channel, bool flag);
		int	getChannelsIndex(std::string channel);
		bool    AuthenticationKeyProcess(const std::string channel, const std::string key);
		int	getClientsIndex(int clientFD);
		void    promotionChannelOperator(std::string channel, int owner, int clientFD);
		void	changeChannel(std::string Name, int clientFD, int flag);
		void	deleteChannel(std::string Name, int clientFD, bool flag = false);
		void    removeOperatorPrivilegesFromEveryBody(std::string channel);
		void    createNewChannel(std::string Name, int clientFD);
		void    kickFromChannel(std::string channel, int owner, int clientFD, std::string message);
		void	addNewClient(int clientFD);
		void    changeTopic(s_commands& com, std::string channelName, int clientFD, std::string topic);
		void	startIRCService(void);
		void	manageBuffers(int index);
		void	shutdownService(void);
		void    sendNoticeMessageToClient(s_commands& com, int clientFD, int clientIndex, std::string message);
		void    sendNoticeMessageToEveryone(s_commands& com, std::string channel, std::string message);
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
		void    whois(s_commands& com);
		void	user(s_commands&);
		bool	handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i);
		void    cap(s_commands& com);
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
		void 	who(s_commands& com);
		void	messageToAllChannels(s_commands&, std::string&);
		void	desconect(s_commands&);
		void	addUserMode(Client* &target, s_commands &com, std::string &sendBuffer, std::map<int, Channel*>* &channels);
		void	topic(s_commands&);
		void	addChannelMode(s_commands &com, Channel* &target, int channelIndex);
		void	newBroadcastAllChannels(s_commands& com, std::string msg, std::string channelName, bool flag);
	public:
		Server(std::string portCheck, std::string password);
		~Server(void);
		void	setPassword(std::string password);
		void	setPort(int Port);
		void	setIsRunning(bool signal);
		void	setServerIRC(int serverFD);
		int	getServerIRC(void) const;
		std::string	getPassword(void) const;
		bool	isKing(int fd) const;
		int	getPort(void)	const;
		int	getNumberOfClients(void) const;
		struct pollfd	(&getPollFds(void))[1024];
		void	setCurrentCommand(s_commands &com);
		s_commands*	getCurrentCommand(void) const;
};
std::ofstream operator<<(std::ostream &out, const Server &other);

bool	isEmptyInput(const std::string &line);
std::map<int, Channel*>* getChannelsMap(void);
std::map<int, Client*>* getClientsMap(void);
struct pollfd(*getMyFds(void))[1024];
bool	*getRunning(void);
bool	findMode(const std::string& myModes, const char mode);
bool	validNick(s_commands& com);
std::string	getLower(const std::string& str);

#endif /* SERVER_HPP */
