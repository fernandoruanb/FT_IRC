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
		it->second->removeMember(clientFD);
		(*clients)[clientFD]->getOperatorChannels().erase(it->second->getName());
		(*clients)[clientFD]->getChannelsSet().erase(it->second->getName());
		(*clients)[clientFD]->getInviteChannels().erase(it->second->getName());
		it->second->getMembersSet().erase(clientFD);
		it->second->getOperatorsSet().erase(clientFD);
		if (it->second->getMembersNum() == 0)
		{
			std::map<int,Channel*>::iterator toErase = it++;
			delete toErase->second;
			channels->erase(toErase);
			continue ;
		}
		++it;
	}
}

void	Server::newBroadcastKill(s_commands& com, std::string msg, std::string complement, std::string channelName, bool flag)
{
	struct pollfd	(&fds)[1024] = *getMyFds();
	std::map<int, Client*>*	clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->begin();
	int	clientsIndex;
	int	channelIndex = getChannelsIndex(channelName);

	if (channelIndex == -1)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + channelName + " :No such channel\r\n";
		return ;
	}
	while (it != clients->end())
	{
		if (it->first == com.fd && flag == true)
		{
			++it;
			continue ;
		}
		if (it->second->getChannelsSet().find(channelName) != it->second->getChannelsSet().end())
		{
			clientsIndex = getClientsIndex(it->first);
			it->second->getBufferOut() += msg + " #" + channelName + " :" + complement;
			fds[clientsIndex].events |= POLLOUT;
		}
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
	std::string	message = "ERROR: You have been killed by " + com.client->getNickName() + " : ";
	int	clientFD;

	if (com.args[1][0] != ':')
	{
		std::cerr << RED "Error: You need to give a message" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " KILL " + ":Not enough parameters" + "\r\n";
		return ;
	}

	std::string complement = getTheMessage(com) + "\r\n";
	message += getTheMessage(com) + "\r\n";

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
	if (this->kingsOfIRC.find(com.fd) == this->kingsOfIRC.end())
	{
		std::cerr << RED "Error: You are not an IRC Operator" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 481 " + com.client->getNickName() + " :Permission Denied- You're not an IRC Operator" + "\r\n";
		return ;
	}

	int	clientsIndex = getClientsIndex(clientFD);

	std::string all = std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " KILL " + (*clients)[clientFD]->getNickName();
	send(fds[clientsIndex].fd, message.c_str(), message.size(), 0);
	(*clients)[clientFD]->setAuthenticated(false);
	(*clients)[clientFD]->setRegistered(false);
	(*clients)[clientFD]->setNickName("*");
	(*clients)[clientFD]->setUserName("*");
	(*clients)[clientFD]->setHost("localhost");
	this->kingsOfIRC.erase(com.fd);
	std::set<std::string>::iterator it = (*clients)[clientFD]->getChannelsSet().begin();
	while (it != (*clients)[clientFD]->getChannelsSet().end())
	{
		newBroadcastKill(com, all, complement, *it, false);
		++it;
	}
	removeAllChannelsOfClient(clientFD);
	(*clients)[clientFD]->getBufferOut().clear();
	sendBuffer[clientsIndex].clear();
	delete (*clients)[clientFD];
	close(fds[clientsIndex].fd);
	fds[clientsIndex].fd = fds[numClients - 1].fd;
	fds[numClients - 1].fd = -1;
	fds[clientsIndex].events = 0;
	this->manageBuffers(clientsIndex);
	this->numClients--;
	clients->erase(clientFD);
	com.isOnline = false;

	std::cout << "The clientFD: " << clientFD << std::endl;
	std::cout << BRIGHT_GREEN "You killed the target" RESET << std::endl;
}
