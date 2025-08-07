#include "../includes/Server.hpp"

static std::vector<std::string>	getAllChannels(s_commands& com)
{
	std::vector<std::string>	channels;
	std::size_t	index = 0;

	while (index < com.args.size())
	{
		if (com.args[index][0] == ':')
			break ;
		if (std::find(channels.begin(), channels.end(), com.args[index]) == channels.end())
		{
			if (com.args[index][0] == '#')
				channels.push_back(com.args[index]);
			else
				channels.push_back(com.args[index]);
		}
		++index;
	}
	return (channels);
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
			return (message);
		}
		++index;
	}
	return (message);
}

void	Server::newBroadcast(s_commands& com, std::string msg, std::string channelName, bool flag)
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
			it->second->getBufferOut() += my_part_message(com.client->getNickName(), com.client->getUserName(), com.client->getHost(), channelName, msg);
			fds[clientsIndex].events |= POLLOUT;
		}
		++it;
	}
}

void	Server::part(s_commands& com)
{
	if (com.args.size() < 1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams(com.client->getNickName(), "PART");
		return ;
	}
	std::vector<std::string> channelsVector = getAllChannels(com);
	std::string	msg = getTheMessage(com);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itm = channels->begin();
	std::string	channelName;
	int	channelIndex;
	std::size_t	index = 0;

	if (msg.empty())
		msg = "I left the channel";

	while (index < channelsVector.size() && !channelsVector[index].empty())
	{
		if (channelsVector[index][0] == '#')
			channelsVector[index] = channelsVector[index].substr(1);
		else
		{
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + channelsVector[index] + " :No such nick/channel\r\n";
			++index;
			continue ;
		}
		channelIndex = getChannelsIndex(channelsVector[index]);
		if (channelIndex == -1 || channelIndex == 0)
		{
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + channelsVector[index] + " :No such nick/channel\r\n";
			++index;
			continue ;
		}
		itm = channels->find(channelIndex);
		channelName = itm->second->getName();
		if (com.client->getChannelsSet().find(itm->second->getName()) != com.client->getChannelsSet().end())
		{
			itm->second->removeMember(com.fd);
			if (itm->second->getMembersNum() == 0)
			{
				com.client->getBufferOut() += my_part_message(com.client->getNickName(), com.client->getUserName(), com.client->getHost(), channelName, msg);
				deleteChannel(channelsVector[index], com.fd);
				++index;
				continue ;
			}
			newBroadcast(com, msg, channelName, false);
			com.client->getOperatorChannels().erase(channelName);
			com.client->getChannelsSet().erase(channelName);
			com.client->getInviteChannels().erase(channelName);
			itm->second->getOperatorsSet().erase(com.fd);
			itm->second->getMembersSet().erase(com.fd);
			com.client->getSendHistory()[channelIndex].clear();
			if (com.client->getOperatorChannels().size() == 0)
				com.client->setIsOperator(false);
			com.client->setChannelOfTime(0);
		}
		else
		{
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 442 " + com.client->getNickName() + " :You're not on that channel\r\n";
		}
		++index;
	}
}
