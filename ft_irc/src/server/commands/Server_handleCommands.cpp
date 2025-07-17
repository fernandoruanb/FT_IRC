#include "../includes/Server.hpp"

static bool	findTheEnd(const char c)
{
	return (c == ' ' || c == '\n' || c == '\r' || c == '\0'
		|| (std::islower(c)));
}

bool	Server::handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i)
{
	s_commands	com(buffer, clients, fd ,i);
	std::map<std::string, void (Server::*)(s_commands&)>	myMap;
	//Add your command here ;)
	myMap["USER"] = &Server::user;
	myMap["PING"] = &Server::handlePing;
	myMap["MODE"] = &Server::mode;

	size_t	j;
	for (j = 0; j < com.line.size(); j++)
		if (findTheEnd(com.line[j]))
			break;
	
	std::string	command = com.line.substr(0, j);

	std::cout << "Comando [" << command << "]" << std::endl;

	if (myMap.find(command) == myMap.end())
		return (false);
	
	struct	pollfd	(&fds)[1024] = *getMyFds();

	(this->*(myMap[command]))(com);
	fds[com.index].events |= POLLOUT;
	return (true);
}
