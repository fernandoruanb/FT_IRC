#include "../includes/Server.hpp"

void	Server::desconect(s_commands& com)
{
	struct pollfd (&fds)[1024] = *getMyFds();

	delete com.client;
	close(fds[com.index].fd);
	fds[com.index].fd = fds[this->numClients - 1].fd;
	fds[this->numClients - 1].fd = -1;
	fds[this->numClients - 1].events = 0;
	--this->numClients;
	com.isOnline = false;
}

std::string	trim(std::string& str)
{
	size_t	start = str.find_first_not_of(' ');
	if (start == std::string::npos)
		return (str);
	size_t	end = str.find_last_not_of(' ');

	return (str.substr(start, end - start + 1));
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
