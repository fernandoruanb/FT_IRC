#include "../includes/Server.hpp"

static std::vector<std::string>	getAllChannels(s_commands& com)
{
	std::vector<std::string>	channels;
	std::size_t	index = 0;

	while (index < com.args.size())
	{
		if (com.args[index][0] == '#')
		{
			if (std::find(channels.begin(), channels.end(), com.args[index].substr(1)) == channels.end())
				channels.push_back(com.args[index].substr(1));
		}
		if (com.args[index][0] == ':')
			break ;
		++index;
	}
	return (channels);
}

static std::vector<std::string>	getAllClients(s_commands& com)
{
	std::vector<std::string>	clients;
	std::size_t	index = 0;

	while (index < com.args.size())
	{
		if (com.args[index][0] == ':')
			break ;
		if (com.args[index][0] != '#')
		{
			if (std::find(clients.begin(), clients.end(), com.args[index]) == clients.end())
				clients.push_back(com.args[index]);
		}
		++index;
	}
	return (clients);
}

static std::string	getTheMessage(s_commands& com)
{
	std::size_t	index = 0;
	std::string	message;

	while (index < com.args.size())
	{
		if (com.args[index][0] == ':')
		{
			message += com.args[index].substr(1);
			message += " ";
			++index;
			while (index < com.args.size())
			{
				message += com.args[index];
				if (index + 1 < com.args.size())
					message += " ";
				++index;
			}
			message += "\r\n";
			return (message);
		}
		++index;
	}
	return (message);
}

void	Server::privmsg(s_commands& com)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::vector<std::string> channelsVector = getAllChannels(com);
	std::vector<std::string> clientsVector = getAllClients(com);
	std::string	message = getTheMessage(com);
	struct pollfd	(&fds)[1024] = *getMyFds();
	int	clientFD;

	if (com.args.size() < 2)
		return ;

	if (channelsVector.empty() && clientsVector.empty())
		return ;
	if (message.empty() || message == " \r\n")
	{
		std::cerr << RED "Error: No text to send" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 412 " + com.client->getNickName() + " :no text to send" + "\r\n";
		return ;
	}
	clientFD = fds[com.index].fd;

	std::size_t	index = 0;
	while (index < channelsVector.size() && !channelsVector[index].empty())
	{
		std::map<int, Client*>::iterator it = clients->find(clientFD);
		std::string	target = channelsVector[index];
		int	targetChannel = getChannelsIndex(target);
		if (targetChannel == -1)
		{
			std::cerr << RED "Error: That channel doesn't exist" RESET << std::endl;
			com.client->getBufferOut() += msg_err_nosuchchannel(com.client->getNickName(), channelsVector[index]);
			++index;
			continue ;
		}
		std::map<int, Channel*>::iterator itm = channels->find(targetChannel);
		std::string	targetChannelName = itm->second->getName();
		if (this->kingsOfIRC.find(it->first) == this->kingsOfIRC.end() && it->second->getChannelsSet().find(targetChannelName) == it->second->getChannelsSet().end())
		{
			std::cerr << RED "Error: You are not on that channel" RESET << std::endl;
			com.client->getBufferOut() += my_notonchannel(com.client->getNickName(), channelsVector[index], "You are not on that channel");
			++index;
			continue ;
		}
		sendBuffer[com.index] += std::string("\n:") + (*clients)[fds[com.index].fd]->getNickName() + "!" + (*clients)[fds[com.index].fd]->getUserName() + "@" + (*clients)[fds[com.index].fd]->getHost() + " PRIVMSG";
		broadcast(com.index, message, targetChannel);
		fds[com.index].events |= POLLOUT;
		++index;
	}
	index = 0;
	int	test = 1;
	std::string	targetChannelName;
	int	targetChannelOfTime;

	while (index < clientsVector.size() && !clientsVector[index].empty())
	{
		test = 1;
		int	targetFD = getClientsFdByName(clientsVector[index]);

		if (targetFD == -1)
		{
			std::cerr << RED "Error: that client doesn't exist" RESET << std::endl;
			com.client->getBufferOut() += my_nosuchnickchannel(com.client->getNickName(), clientsVector[index]);
			++index;
			continue ;
		}
		while (fds[test].fd != targetFD)
			++test;
		if (test == numClients || test == -1)
		{
			std::cerr << RED "Error: The client doesn't exist" RESET << std::endl;
			com.client->getBufferOut() += my_nosuchnickchannel(com.client->getNickName(), clientsVector[index]);
			++index;
			continue ;
		}
		targetChannelOfTime = (*clients)[targetFD]->getChannelOfTime();
		targetChannelName = (*channels)[targetChannelOfTime]->getName();
		if (std::find(channelsVector.begin(), channelsVector.end(), targetChannelName) != channelsVector.end())
		{
			++index;
			continue ;
		}
		privmsg(test, com.index, message);
		++index;
	}
}
