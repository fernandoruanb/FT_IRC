#include "../includes/Server.hpp"

static std::string	getNames(Channel* channel, s_commands& com, bool canSee)
{
	std::set<int>	members = channel->getMembersSet();
	std::string		result;

	std::set<int>::const_iterator it;
	for (it = members.begin(); it != members.end(); it++)
	{
		int	fd = *it;
		Client	*client = NULL;

		std::map<int, Client*>::iterator	cit = com.clients->find(fd);
		if (cit != com.clients->end())
			client = cit->second;
		(void)canSee;
		if (client && (!client->getIsInvisible()))
			result += client->getNickName() + " ";
	}
	
	std::set<int>	operators = channel->getOperatorsSet();
	std::set<int>::const_iterator	oit;
	for (oit = operators.begin(); oit != operators.end(); oit++)
	{
		int fd = *it;
		Client*	client = NULL;

		std::map<int, Client*>::iterator	cit = com.clients->find(fd);
		if (cit != com.clients->end())
			client = cit->second;
	
		if (client && (!client->getIsInvisible()))
			result += client->getNickName() + " ";
	}

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
