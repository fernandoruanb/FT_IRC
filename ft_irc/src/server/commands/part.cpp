#include "../includes/Server.hpp"

static std::vector<std::string>	getAllChannels(s_commands& com)
{
	std::vector<std::string>	channels;
	std::size_t	index = 0;

	while (index < com.args.size())
	{
		if (com.args[index][0] == ':')
			break ;
		if (std::find(channels.begin(), channels.end(), com.args[index].substr(1)) == channels.end())
		{
			if (com.args[index][0] == '#')
				channels.push_back(com.args[index].substr(1));
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
			message += "\r\n";
			return (message);
		}
		++index;
	}
	return (message);
}

void	Server::part(s_commands& com)
{
	if (com.args.size() < 1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams("PART");
		return ;
	}
	std::vector<std::string> channelsVector = getAllChannels(com);
	std::string	msg = getTheMessage(com);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itm = channels->begin();
	std::string	channelName;
	int	channelIndex;
	std::size_t	index = 0;

	if (msg.empty() || msg == " \r\n")
		msg = "You left the channel";

	while (index < channelsVector.size() && !channelsVector[index].empty())
	{
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
			com.client->getBufferOut() += my_part_message(com.client->getNickName(), com.client->getUserName(), com.client->getHost(), channelName, msg);
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
		++index;
	}
}
