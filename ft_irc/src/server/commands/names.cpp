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
		
		if (client && (canSee || !findMode(client->getMode(com.client->getChannelOfTime()), 'i')))
			result += client->getNickName() + " ";
	}

	result += channel->getOperatorsNames();

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
	// callCmdMsg(names, 353, com, com.sendBuffer);
	com.sendBuffer += msg_353(nick, channelName, names);
	com.sendBuffer += msg_366(nick, channelName);
}

/*
	List all users in a channel

	if use NAMES - no arguments
		shows all user in public channels
	
	Sintaxes
		NAMES #channel
		NAMES #channel,#channel2,#etc...

		no spaces afters comas
*/
void	Server::names(s_commands& com)
{
	bool	canSee = (
		this->isKing(com.client->getClientFD())
		|| findMode(com.client->getMode(com.client->getChannelOfTime()), 'o')
	);

	if (com.args.empty())
		return (showAllNames(com, this->channels, canSee));
	showChannel(com, this->channels, canSee);

}
