#include "../includes/Server.hpp"

static bool	setCanSee(Client* caller, Client *other, bool isOper, Channel* channel)
{
	int	callerFd = caller->getClientFD();
	int	otherFd = other->getClientFD();

	bool	sameChannel =
		(channel->isMemberOfChannel(callerFd) || channel->isOperatorOfChannel(callerFd))
		&& (channel->isMemberOfChannel(otherFd) || channel->isOperatorOfChannel(otherFd));

	return (isOper || caller->getNickName() == other->getNickName() || sameChannel);
}

static std::string	fillResult(const std::set<int> &members, s_commands& com, bool canSee, Channel* channel, bool flag = false)
{
	std::set<int>::const_iterator it;
	std::string	result;

	for (it = members.begin(); it != members.end(); it++)
	{
		int	fd = *it;
		Client	*client = NULL;
		bool	myCanSee = false;

		std::map<int, Client*>::iterator	cit = com.clients->find(fd);
		if (cit != com.clients->end())
			client = cit->second;
		
		if (client)
		{
			myCanSee = setCanSee(com.client, client, canSee, channel);
			if ((!client->getIsInvisible()) || myCanSee)
				result += (flag ? "@" : "") + client->getNickName() + " ";
		}
	}

	return (result);
}

static std::string	getNames(Channel* channel, s_commands& com, bool canSee)
{
	std::string		result;

	if (!channel)
		return (result);

	std::set<int>	members = channel->getMembersSet();
	

	result = fillResult(members, com, canSee, channel);
	result += fillResult(channel->getOperatorsSet(), com, canSee, channel, 1);
	
	return (result);
}

static void	showAllNames(s_commands& com, std::map<int, Channel*>* &channels, bool canSee)
{
	std::string	names;
	std::string	nick = com.client->getNickName();
	std::string	channelName;

	std::map<int, Channel*>::iterator it;
	for (it = channels->begin(); it != channels->end(); it++)
	{
			channelName = it->second->getName();
			names = getNames(it->second, com, canSee);
			com.sendBuffer += msg_353(nick, channelName, names);
			com.sendBuffer += msg_366(nick, channelName);
	}
}

static void	showChannel(s_commands& com, std::map<int, Channel*>* &channels, bool canSee)
{
	std::string	channelName = com.args[0].substr(1);
	Channel	*channel = NULL;
	std::string	nick = com.client->getNickName();

	std::map<int, Channel*>::iterator it;
	for (it = channels->begin(); it != channels->end(); it++)
		if (it->second->getName() == channelName)
		{
			channel = it->second;
			break;
		}

	if (!channel)
	{
		com.sendBuffer += my_nosuchnickchannel(nick, channelName);
		return;
	}
	
	std::string	names = getNames(channel, com, canSee);
	com.sendBuffer += msg_353(nick, channelName, names);
	com.sendBuffer += msg_366(nick, channelName);
}

void	Server::names(s_commands& com)
{
	bool	canSee = (
		this->isKing(com.client->getClientFD())
	);

	if (com.args.empty())
		return (showAllNames(com, this->channels, canSee));
	showChannel(com, this->channels, canSee);

}
