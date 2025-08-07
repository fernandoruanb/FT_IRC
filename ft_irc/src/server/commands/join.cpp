#include "../includes/Server.hpp"

static bool	validPass(Channel* &channel, const std::string& pass)
{
	return (!findMode(channel->getMode(), 'k')
			|| channel->getPassWord() == pass);
}

static Channel*	getChannel(const std::string& chanName, std::map<int, Channel*>* &channels)
{
	std::map<int, Channel*>::iterator	it;

	for (it = channels->begin(); it != channels->end(); it++)
		if (it->second->getName() == chanName)
			return (it->second);
	
	return (NULL);
}

static void	getArgs(s_commands& com, std::vector<std::string>& myChannels, std::vector<std::string>& myPass)
{
	std::vector<std::string>::iterator	it = com.args.begin();

	while (it != com.args.end() && !(*it).empty() && (*it)[0] == '#')
	{
		myChannels.push_back(*it);
		it++;
	}
	
	while (it != com.args.end())
	{
		myPass.push_back(*it);
		it++;
	}
}

void	Server::join(s_commands& com)
{
	Channel*	firstChannel = NULL;
	size_t		passIndex = 0;
	bool		create = false;
	std::string	firstPass;
	Channel*	currentChannel = NULL;
	std::vector<std::string>	channelsArgs;
	std::vector<std::string>	passArgs;
	
	if (com.args.size() < 1)
	{
		this->sendBuffer[com.index] += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " JOIN " + ":Not enough parameters\r\n";
		return ;
	}
	if (com.args[0].empty() || com.args[0][0] != '#')
	{
		this->sendBuffer[com.index] += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + com.args[0] + " :No such nick/channel\r\n";
		return ;
	}
	setCurrentCommand(com);

	getArgs(com, channelsArgs, passArgs);

	for (size_t i = 0; i < channelsArgs.size(); i++)
	{
		std::string	name = channelsArgs[i].substr(1);
		name = getLower(name);

		currentChannel = getChannel(name, this->channels);
		if (!currentChannel)
		{
			createNewChannel(name, com.fd);
			currentChannel = getChannel(name, this->channels);
			create = true;
		}
		if (!firstChannel)
		{
			if (findMode(currentChannel->getMode(), 'k'))
			{
				firstPass = currentChannel->getPassWord();
				passIndex = i;
			}
			firstChannel = currentChannel;
		}

		if (i < passArgs.size() && !passArgs[i].empty())
		{
			if (!validPass(currentChannel, passArgs[i]))
			{
				com.sendBuffer += msg_badchannelkey(com.client->getNickName(), currentChannel->getName());
				continue;
			}
		}
		if (!firstPass.empty())
		{
			if (!passArgs.empty() &&  firstPass == passArgs[passIndex])
				changeChannel(firstChannel->getName(), com.fd, 1);
			else
				com.sendBuffer += msg_badchannelkey(com.client->getNickName(), currentChannel->getName());
			return;
		}
		if (currentChannel->isMemberOfChannel(com.fd))
			return;
		if (!create)
		{
			if (com.client->getChannelsSet().find(currentChannel->getName()) != com.client->getChannelsSet().end())
				changeChannel(currentChannel->getName(), com.fd, 2);
			else
			{
				// std::string messageToEveryone = std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " JOIN " +  "#" + currentChannel->getName() + "\r\n";
				// newBroadcastAllChannels(com, messageToEveryone, currentChannel->getName(), true);
				changeChannel(currentChannel->getName(), com.fd, 1);
			}
		}
	}
}
