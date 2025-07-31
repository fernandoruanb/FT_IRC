#include "../includes/Server.hpp"

static std::string	getTheMessage(s_commands& com)
{
	std::size_t	index = 0;
	std::string	message;

	while (index < com.args.size() && com.args[index][0] != ':')
		++index;
	message += com.args[index].substr(1);
	if (index + 1 < com.args.size())
		message += " ";
	++index;
	while (index < com.args.size())
	{
		message += com.args[index];
		if (index + 1 < com.args.size())
			message += " ";
		++index;
	}
	return (message);
}

void	Server::removeAllChannelsOfClient(int clientFD)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>::iterator it = channels->begin();

	this->kingsOfIRC.erase(clientFD);
	while (it != channels->end())
	{
		(*clients)[clientFD]->getOperatorChannels().erase(it->second->getName());
		(*clients)[clientFD]->getChannelsSet().erase(it->second->getName());
		(*clients)[clientFD]->getInviteChannels().erase(it->second->getName());
		it->second->getMembersSet().erase(clientFD);
		it->second->getOperatorsSet().erase(clientFD);
		++it;
	}
}

void	Server::kill(s_commands& com)
{
	if (com.args.size() < 2)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " KILL " + ":Not enough parameters" + "\r\n";
		return ;
	}
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::string	nick = com.args[0];
	std::string	message = getTheMessage(com);
	int	clientFD;
	int	clientIndex;

	if (com.args[1][0] != ':')
	{
		std::cerr << RED "Error: You need to give a message" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " KILL " + ":Not enough parameters" + "\r\n";
		return ;
	}

	if (message.empty() || message == " \r\n")
	{
		std::cerr << RED "Error: no text to send" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 412 " + com.client->getNickName() + " :no text to send" + "\r\n";
		return ;
	}
	clientFD = getClientsFdByName(nick);
	if (clientFD == -1)
	{
		std::cerr << RED "Error: The client doesn't exist to be KILL" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 401 " + com.client->getNickName() + " " + nick + " :No such nick/channel" + "\r\n";
		return ;
	}
	if (com.fd == clientFD)
	{
		std::cerr << RED "Error: You are trying to KILL yourself" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 400 " + com.client->getNickName() + " KILL " + ":cannot kill yourself" + "\r\n";
		return ;
	}
	clientIndex = getClientsIndex(clientFD);
	if (this->kingsOfIRC.find(com.fd) == this->kingsOfIRC.end())
	{
		std::cerr << RED "Error: You are not an IRC Operator" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 481 " + com.client->getNickName() + " :Permission Denied- You're not an IRC Operator" + "\r\n";
		return ;
	}

	removeAllChannelsOfClient(clientFD);
	(*clients)[clientFD]->setAuthenticated(false);
	(*clients)[clientFD]->setRegistered(false);
	(*clients)[clientFD]->setNickName("*");
	(*clients)[clientFD]->setUserName("*");
	(*clients)[clientFD]->setHost("localhost");

	std::cout << "The clientFD: " << clientFD << std::endl;
	(*clients)[clientFD]->getBufferOut() += std::string("ERROR :You have been killed: ") + message + "\r\n";
	std::cout << BRIGHT_GREEN "You killed the target" RESET << std::endl;
	fds[clientIndex].events |= POLLOUT;
}
