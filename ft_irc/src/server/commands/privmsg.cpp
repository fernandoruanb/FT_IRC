#include "../includes/Server.hpp"

void	Server::privmsg(s_commands& com)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	struct pollfd	(&fds)[1024] = *getMyFds();
	int	clientFD;

	if (com.args.size() < 2)
		return ;
	if (com.args[1][0] != ':')
		return ;

	std::string	message = com.args[1].substr(1, com.args[1].size());
	clientFD = fds[com.index].fd;
	std::size_t	index = 2;

	if (index + 1 < com.args.size())
		message += " ";
	while (index < com.args.size())
	{
		message += com.args[index];
		if (index + 1 < com.args.size())
			message += " ";
		++index;
	}
	message += "\r\n";
	if (com.args[0][0] == '#')
	{
		std::map<int, Client*>::iterator it = clients->find(clientFD);
		std::string	target = com.args[0].substr(1, com.args[0].size());
		int	targetChannel = getChannelsIndex(target);
		if (targetChannel == -1)
		{
			std::cerr << RED "Error: That channel doesn't exist" RESET << std::endl;
			com.client->getBufferOut() += msg_err_nosuchchannel(com.client->getNickName(), com.args[0].substr(1));
			return ;
		}
		std::map<int, Channel*>::iterator itm = channels->find(targetChannel);
		std::string	targetChannelName = itm->second->getName();
		if (it->second->getChannelsSet().find(targetChannelName) == it->second->getChannelsSet().end())
		{
			std::cerr << RED "Error: You are not on that channel" RESET << std::endl;
			com.client->getBufferOut() += my_notonchannel(com.client->getNickName(), com.args[0].substr(1), "You are not on that channel");
			return ;
		}
		sendBuffer[com.index] += std::string("\n:") + (*clients)[fds[com.index].fd]->getNickName() + "!" + (*clients)[fds[com.index].fd]->getUserName() + "@" + (*clients)[fds[com.index].fd]->getHost() + " PRIVMSG";
		broadcast(com.index, message, targetChannel);
		fds[com.index].events |= POLLOUT;
		return ;
	}
	else
	{
		int	targetFD = getClientsFdByName(com.args[0]);

		if (targetFD == -1)
		{
			std::cerr << RED "Error: that client doesn't exist" RESET << std::endl;
			com.client->getBufferOut() += my_nosuchnickchannel(com.client->getNickName(), com.args[0]);
			return ;
		}
		int	index = 1;
		while (fds[index].fd != targetFD)
			++index;
		if (index == numClients || index == -1)
		{
			std::cerr << RED "Error: The client doesn't exist" RESET << std::endl;
			com.client->getBufferOut() += my_nosuchnickchannel(com.client->getNickName(), com.args[0]);
			return ;
		}
		privmsg(index, com.index, message);
	}
}
