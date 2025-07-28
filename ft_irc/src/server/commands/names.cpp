#include "../includes/Server.hpp"

static std::string	getNames(Channel* channel, s_commands& com)
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
		
		if (client && !findMode(client->getMode(com.client->getChannelOfTime()), 'i'))
			result += client->getNickName() + " ";
	}

	result += channel->getOperatorsNames();

	return (result);
}

static void	showAllNames(s_commands& com, std::map<int, Channel*>* &channels)
{
	std::string	names;
	std::string	prefix;

	std::map<int, Channel*>::iterator it;
	for (it = channels->begin(); it != channels->end(); it++)
	{
		prefix = it->second->getName();
		names = prefix + ": " + getNames(it->second, com);
		com.sendBuffer += "Animais presentes no canal #" + names + "\n";
	}
}

static void	showChannel(s_commands& com, std::map<int, Channel*>* &channels)
{
	std::string	channelName = com.args[0].substr(1);
	Channel	*channel = NULL;

	std::map<int, Channel*>::iterator it;
	for (it = channels->begin(); it != channels->end(); it++)
		if (it->second->getName() == channelName)
		{
			channel = it->second;
			break;
		}

	if (!channel)
		return (callCmdMsg("No such channel", 401, com, com.sendBuffer));
	
	std::string	names = getNames(channel, com);

	callCmdMsg(names, 353, com, com.sendBuffer);
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
	if (com.args.empty())
		return (showAllNames(com, this->channels));
	showChannel(com, this->channels);

}
