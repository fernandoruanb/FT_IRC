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
/*
	Sintax

		JOIN #canal senha123
		||
		JOIN #canal1,#canal2 senha1, senha2
*/
void	Server::join(s_commands& com)
{
	Channel*	firstChannel = NULL;
	size_t		passIndex = 0;
	std::string	firstPass;
	Channel*	currentChannel = NULL;
	std::vector<std::string>	channelsArgs;
	std::vector<std::string>	passArgs;

	if (com.args.size() < 1)
		return ;
	if (com.args[0].empty() || com.args[0][0] != '#')
	{
		this->sendBuffer[com.index] += msg_err_nosuchchannel(com.client->getNickName(), com.args[0]);
		return ;
	}

	getArgs(com, channelsArgs, passArgs);

	for (size_t i = 0; i < channelsArgs.size(); i++)
	{
		std::string	name = channelsArgs[i].substr(1);

		currentChannel = getChannel(name, this->channels);
		if (!currentChannel)
		{
			createNewChannel(name, com.fd);
			currentChannel = getChannel(name, this->channels);
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
				com.sendBuffer += "Senha invalida\n";
				continue;
			}
		}
		currentChannel->addNewMember(com.fd);

		std::cout << "Membros do " + currentChannel->getName() + ": " + currentChannel->getClientsNames() << std::endl;
	}

	if (!firstPass.empty())
	{
		if (!passArgs.empty() &&  firstPass == passArgs[passIndex])
			changeChannel(firstChannel->getName(), com.fd, 0);
		else
			com.sendBuffer += "Senha invalida do canal " + firstChannel->getName() + "\n";
		return;
	}
	std::cout << "Membros do " + firstChannel->getName() + ": " + firstChannel->getClientsNames() << std::endl;
	changeChannel(firstChannel->getName(), com.fd, 0);
	std::cout << "Membros do " + firstChannel->getName() + ": " + firstChannel->getClientsNames() << std::endl;
}
