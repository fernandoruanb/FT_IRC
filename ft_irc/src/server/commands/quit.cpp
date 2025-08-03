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

void	Server::messageToAllChannels(s_commands& com, const std::string& message)
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

	for (it = this->channels->begin(); it != this->channels->end(); it++)
	{
		Channel*	channel = it->second;
		std::set<std::string>::iterator	sit;

		for (sit = myChannels.begin(); sit != myChannels.end(); sit++)
			if (*sit == channel->getName())
			{
				int	index = this->getChannelsIndex(channel->getName());
				com.sendBuffer = message;
				this->broadcast(com.index, message, index);
				break;
			}
	}
}

void	Server::quit(s_commands& com)
{
	if (com.args.size())
	{
		std::string line;
		std::string	message = ":" + com.client->getNickName() + " ";

		for (size_t i = 0; i < com.args.size(); i++)
			message += com.args[i] + ' ';
		
		message += '\n';
		messageToAllChannels(com, message);
	}
	desconect(com);
}
