#include "../includes/Server.hpp"

static bool	findTheEnd(const char c)
{
	return ((c < 'A' || c > 'Z'));
}

bool	Server::handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i)
{
	std::map<std::string, void (Server::*)(s_commands&)>	myMap;
	myMap["USER"] = &Server::user;
	myMap["PING"] = &Server::handlePing;
	myMap["MODE"] = &Server::mode;
	myMap["NICK"] = &Server::nick;
	myMap["INVITE"] = &Server::invite;
	myMap["JOIN"] = &Server::join;
	myMap["KICK"] = &Server::kick;
	myMap["KILL"] = &Server::kill;
	myMap["LIST"] = &Server::list;
	myMap["MOTD"] = &Server::motd;
	myMap["NAMES"] = &Server::names;
	myMap["NOTICE"] = &Server::notice;
	myMap["OPER"] = &Server::oper;
	myMap["PART"] = &Server::part;
	myMap["PASS"] = &Server::pass;
	myMap["PRIVMSG"] = &Server::privmsg;
	myMap["QUIT"] = &Server::quit;
	myMap["TOPIC"] = &Server::topic;
	myMap["CAP"] = &Server::cap;
	myMap["WHOIS"] = &Server::whois;
	myMap["WHO"] = &Server::who;

	size_t	j;
	for (j = 0; j < buffer.size(); j++)
		if (findTheEnd(buffer[j]))
			break;
	
	struct	pollfd	(&fds)[1024] = *getMyFds();
	std::string	command = buffer.substr(0, j);
	
	if (myMap.find(command) == myMap.end())
		return (false);

	while (buffer[j] == ' ')
		j++;
	std::string	arguments = buffer.substr(j);
	s_commands	com(buffer, clients, fd ,i, arguments, command, this->sendBuffer[i]);
	bool	signin = com.client->getRegistered();

	if (!com.client->getAuthenticated())
	{
		if (command != "CAP" && command != "PASS" && command != "QUIT")
		{
			com.client->getBufferOut() += msg_err_notregistered();
			fds[com.index].events |= POLLOUT;
			return (false);
		}
	}
	else if (!com.client->getRegistered())
	{
		const std::string	allowed[4] = {"CAP", "USER", "NICK", "QUIT"};
		bool	isValid = false;

		for (size_t i = 0; i < allowed->size(); i++)
			if (command == allowed[i])
			{
				isValid = true;
				break;
			}
		
		if (!isValid)
		{
			com.client->getBufferOut() += msg_err_notregistered();
			fds[com.index].events |= POLLOUT;
			return (false);
		}
	}
	
	if (com.client->getAuthenticated() && com.client->getRegistered())
		com.isOnline = true;

	(this->*(myMap[command]))(com);

	if (com.isOnline)
	{
		if (com.client->getAuthenticated() && com.client->getRegistered() && com.client->getChannelsSet().find("generic") == com.client->getChannelsSet().end())
		{
			//std::string messageToEveryone = std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " JOIN " + "#generic" + "\r\n";
			changeChannel("generic", com.fd, 1);
			com.client->getChannelsSet().insert("generic");
			com.client->setChannelOfTime(0);
			//newBroadcastAllChannels(com, messageToEveryone, "generic", true);
		}

		if (!signin && com.client->getRegistered())
			com.sendBuffer +=
				msg_welcome(com.client)
				+ msg_yourhost(com.client->getNickName())
				+ msg_created(com.client->getNickName())
				+ msg_svrinfo(com.client->getNickName())
				+ ":" SERVER_NAME " 375 " + com.client->getNickName() + " :- Message of the day -\r\n"
				+ ":" SERVER_NAME " 372 " + com.client->getNickName() + " :- Welcome to our IRC server!\r\n"
				+ ":" SERVER_NAME " 372 " + com.client->getNickName() + " :- Respect the rules.\r\n"
				+ ":" SERVER_NAME " 376 " + com.client->getNickName() + " :End of /MOTD command.\r\n";
	}

	fds[com.index].events |= POLLOUT;
	return (true);
}
