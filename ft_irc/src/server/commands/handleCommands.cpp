#include "../includes/Server.hpp"

static bool	findTheEnd(const char c)
{
	// return (c == ' ' || c == '\n' || c == '\r' || c == '\0'
	// 	|| (std::islower(c)));
	return ((c < 'A' || c > 'Z'));
}

bool	Server::handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i)
{
	std::map<std::string, void (Server::*)(s_commands&)>	myMap;
	/*
		Add your commands here
		
		NOTE: you dont need to check if the buffer has the command in your function
			this function already do it
	*/
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

	size_t	j;
	for (j = 0; j < buffer.size(); j++)
		if (findTheEnd(buffer[j]))
			break;
	
	struct	pollfd	(&fds)[1024] = *getMyFds();
	std::string	command = buffer.substr(0, j);

	//log for debug
	std::cout << "Comando [" << command << "]" << std::endl;
	
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
	
	//log for debug
	std::cout << "argc: " << com.args.size() << std::endl;
	std::cout << "arguments: " << arguments << std::endl;

	for (size_t i = 0;  i < com.args.size(); i++)
		std::cout << "My args array[" << i << "]: " << com.args[i] << std::endl;

	if (com.client->getAuthenticated() && com.client->getRegistered())
		com.isOnline = true;

	(this->*(myMap[command]))(com);

	if (com.isOnline)
	{
		std::cout << "getRegistred: " << com.client->getRegistered() << std::endl;
		if (com.client->getAuthenticated() && com.client->getRegistered() && com.client->getChannelsSet().find("generic") == com.client->getChannelsSet().end())
		{
			changeChannel("generic", com.fd, 1);
			com.client->getChannelsSet().insert("generic");
			com.client->setChannelOfTime(0);
		}

		if (!signin && com.client->getRegistered())
			com.sendBuffer +=
				msg_welcome(com.client)
				+ msg_yourhost(com.client->getNickName())
				+ msg_created(com.client->getNickName())
				+ msg_svrinfo(com.client->getNickName());
	}

	fds[com.index].events |= POLLOUT;
	// Limpa o buffer do cliente após processar comando válido
	// this->recvBuffer[com.index].clear();
	return (true);
}
