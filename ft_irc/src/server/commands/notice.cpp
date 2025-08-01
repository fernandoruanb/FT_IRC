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

void	Server::sendNoticeMessageToEveryone(s_commands& com, std::string channel, std::string message)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->begin();
	bool	theChannelTest = com.client->getChannelsSet().find(channel) != com.client->getChannelsSet().end();
	int	test;

	while (theChannelTest && it != clients->end())
	{
		test = 1;
		if (checkCompatibility(com.fd, it->first, channel))
		{
			if (it->first == com.fd)
			{
				++it;
				continue ;
			}
			it->second->getBufferOut() += std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " NOTICE " + "#" + channel + " :" + message;
			while (fds[test].fd != it->first)
				++test;
			fds[test].events |= POLLOUT;
		}
		++it;
	}
}

void	Server::sendNoticeMessageToClient(s_commands& com, int clientFD, int clientIndex, std::string message)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	struct pollfd (&fds)[1024] = *getMyFds();
	int	ownerChannelOfTime = com.client->getChannelOfTime();
	std::string	channelName = (*channels)[ownerChannelOfTime]->getName();

	if (clientFD == -1)
		return ;
	if (clientIndex == -1)
		return ;

	if (checkCompatibility(com.fd, clientFD, "generic"))
	{
		(*clients)[clientFD]->getBufferOut() += std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " NOTICE " + (*clients)[clientFD]->getNickName() + " :" + message;
		fds[clientIndex].events |= POLLOUT;
	}
}

void	Server::notice(s_commands& com)
{
	std::vector<std::string> channelsVector = getAllChannels(com);
	std::vector<std::string> clientsVector = getAllClients(com);
	std::string	message = getTheMessage(com);
	std::size_t	index = 0;

	if (channelsVector.empty() && clientsVector.empty())
	{
		std::cerr << RED "Error: we don't have anyone to send a NOTICE" RESET << std::endl;
		return ;
	}
	if (message.empty() || message == " \r\n")
	{
		std::cerr << RED "Error: you need to give a message to do it" RESET << std::endl;
		return ;
	}
	while (index < channelsVector.size() && !channelsVector[index].empty())
	{
		sendNoticeMessageToEveryone(com, channelsVector[index], message);
		++index;
	}
	index = 0;
	while (index < clientsVector.size() && !clientsVector[index].empty())
	{
		int	clientFD = getClientsFdByName(clientsVector[index]);
		int	clientIndex = getClientsIndex(clientFD);
		sendNoticeMessageToClient(com, clientFD, clientIndex, message);
		++index;
	}
}
