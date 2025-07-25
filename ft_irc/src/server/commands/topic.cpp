#include "../includes/Server.hpp"

void	Server::topic(s_commands& com)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>::iterator it = clients->find(com.fd);
	std::map<int, Channel*>::iterator itm = channels->begin();
	std::string	channel;
	std::string	topic;
	std::size_t	index;

	if (com.args.size() < 1)
		return ;
	if (com.args.size() == 1 && com.args[0][0] == '#')
	{
		channel = com.args[0].substr(1, com.args[0].size());
		int channelIndex = getChannelsIndex(channel);
		if (channelIndex == -1)
			return ;
		itm = channels->find(channelIndex);
		it->second->getBufferOut() += my_join_rpl_topic(it->second->getNickName(), itm->second->getName(), itm->second->getTopic());
		fds[com.index].events |= POLLOUT;
		return ;
	}
	else if (com.args[0][0] == '#' && com.args[1][0] == ':')
	{
		channel = com.args[0].substr(1, com.args[0].size());
		topic = com.args[1].substr(1, com.args[1].size());
		index = 2;
		if (index + 1 < com.args.size())
			topic += " ";
		while (index < com.args.size())
		{
			topic += com.args[index];
			if (index + 1 < com.args.size())
				topic += " ";
			++index;
		}
		changeTopic(channel, com.fd, topic);
		fds[com.index].events |= POLLOUT;
		return ;
	}
}
