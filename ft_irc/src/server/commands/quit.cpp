#include "../includes/Server.hpp"

void	Server::desconect(s_commands& com)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int,Client*>* clients = getClientsMap();

	removeAllChannelsOfClient(com.fd);
	(*clients)[com.fd]->setAuthenticated(false);
	(*clients)[com.fd]->setRegistered(false);
	(*clients)[com.fd]->setNickName("*");
	(*clients)[com.fd]->setUserName("*");
	(*clients)[com.fd]->setHost("localhost");	
	com.isOnline = false;
	this->kingsOfIRC.erase(com.fd);
	delete com.client;
	close(fds[com.index].fd);
	clients->erase(com.fd);
	fds[com.index].fd = fds[numClients - 1].fd;
	fds[numClients - 1].fd = -1;
	fds[numClients - 1].events = 0;
	this->manageBuffers(com.index);
	this->numClients--;
}

std::string	trim(std::string& str)
{
	size_t	start = str.find_first_not_of(' ');
	if (start == std::string::npos)
		return (str);
	size_t	end = str.find_last_not_of(' ');

	return (str.substr(start, end - start + 1));
}
void	Server::newBroadcastAllChannels(s_commands& com, std::string msg, std::string channelName, bool flag)
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
			it->second->getBufferOut() += msg;
			fds[clientsIndex].events |= POLLOUT;
		}
		++it;
	}
}

void	Server::messageToAllChannels(s_commands& com, std::string& message)
{
	/*
		Vms mandar a mensagem para tds os canais do com.client

		esse std::set<std::string> tem os nomes dos canais, vms procurar eles e mandar broadcast
			com.client->getChannels();
			this->channels;
			broadcast(com.index, message, <index do canal>);
	*/

	std::map<int, Channel*>::iterator	it;
	std::set<std::string>&	myChannels = com.client->getChannelsSet();
	message = trim(message) + "\r\n";

	for (it = this->channels->begin(); it != this->channels->end(); it++)
	{
		Channel*	channel = it->second;
		std::set<std::string>::iterator	sit;

		for (sit = myChannels.begin(); sit != myChannels.end(); sit++)
			if (*sit == channel->getName())
			{
				int	index = this->getChannelsIndex(channel->getName());
				com.sendBuffer = message;
				this->newBroadcastAllChannels(com, message, channel->getName(), false);
				break;
			}
	}
}

void	Server::quit(s_commands& com)
{
	if (com.args[0][0] == ':')
	{
		std::string line;
		std::string	message;
		com.args[0] = com.args[0].substr(1);

		for (size_t i = 0; i < com.args.size(); i++)
			message += com.args[i] + ' ';
		
		message = msg_quit(com, message);
		messageToAllChannels(com, message);
	}
	desconect(com);
}
