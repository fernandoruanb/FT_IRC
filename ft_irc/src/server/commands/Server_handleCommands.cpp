#include "../includes/Server.hpp"

static bool	findTheEnd(const char c)
{
	return (c == ' ' || c == '\n' || c == '\r' || c == '\0'
		|| (std::islower(c)));
}

bool	Server::handleCommands(std::map<int, Client*>* &clients, std::string& buffer, int fd, int i)
{
	std::map<std::string, void (Server::*)(s_commands&)>	myMap;
	//Add your command here ;)
	myMap["USER"] = &Server::user;
	myMap["PING"] = &Server::handlePing;
	myMap["MODE"] = &Server::mode;
	myMap["NICK"] = &Server::nick;

	size_t	j;
	for (j = 0; j < buffer.size(); j++)
		if (findTheEnd(buffer[j]))
			break;
	
	std::string	command = buffer.substr(0, j);

	std::cout << "Comando [" << command << "]" << std::endl;
	
	if (myMap.find(command) == myMap.end())
		return (false);
	
	struct	pollfd	(&fds)[1024] = *getMyFds();
	while (buffer[j] == ' ')
		j++;
	std::string	arguments = buffer.substr(j);
	s_commands	com(buffer, clients, fd ,i, arguments);

	std::cout << "argc: " << com.args.size() << std::endl;
	std::cout << "arguments: " << arguments << std::endl;
	for (size_t i = 0;  i < com.args.size(); i++)
		std::cout << "My args array[" << i << "]: " << com.args[i] << std::endl;

	(this->*(myMap[command]))(com);
	fds[com.index].events |= POLLOUT;
	return (true);
}
