#include "../includes/Server.hpp"
#include <sstream>

s_mode::s_mode(char s, char f, bool ff, Channel* &t, std::string& c, size_t l, int i, bool k)
	: sign(s), flag(f), flagFound(ff), target(t), currentMode(c), len(l), channelIndex(i), isKing(k)
{}

int     getChannelsIndexMode(std::string channel)
{
        channel = getLower(channel);
        std::map<int, Channel*>* channels = getChannelsMap();
        std::map<int, Channel*>::iterator itch = channels->begin();

        while (itch != channels->end())
        {
                if (itch->second->getName() == channel)
                        return (itch->first);
                ++itch;
        }
        return (-1);
}

int     getClientsIndexMode(int clientFD)
{
        struct pollfd (&fds)[1024] = *getMyFds();
        int     index;

        index = 1;
        while (index < 1024 && fds[index].fd != -1)
        {
                if (fds[index].fd == clientFD)
                        return (index);
                ++index;
        }
        return (-1);
}

void	newBroadcastAllChannelsMode(s_commands& com, std::string msg, std::string channelName, bool flag)
{
	struct pollfd	(&fds)[1024] = *getMyFds();
	std::map<int, Client*>*	clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->begin();
	int	clientsIndex;
	int	channelIndex = getChannelsIndexMode(channelName);

	if (channelIndex == -1)
	{
        if (!com.client || clients->find(com.fd) == clients->end())
            return;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + channelName + " :No such channel\r\n";
		return ;
	}
	while (it != clients->end())
	{
        if (!it->second || clients->find(it->first) == clients->end()) {
            ++it;
            continue;
        }
		if (it->first == com.fd && flag == true)
		{
			++it;
			continue ;
		}
      	if (it->second->getChannelsSet().find(channelName) != it->second->getChannelsSet().end())
        {
            clientsIndex = getClientsIndexMode(it->first);
            if (clientsIndex >= 0) {
                it->second->getBufferOut() += msg;
                fds[clientsIndex].events |= POLLOUT;
            }
        }
		++it;
	}
}

static bool	isSigned(const char c)
{
	return (c == '+' || c == '-');
}

static	Client	*getTargetClient(s_commands &com, const std::string& name)
{
	std::map<int, Client*>::iterator	it;

	for (it = com.clients->begin(); it != com.clients->end(); it++)
		if (it->second->getNickName() == name)
			return (it->second);
	
	com.sendBuffer += my_nosuchnickchannel(com.client->getNickName(), name);
	return (NULL);
}

static Channel*	getTargetChannel(s_commands &com, std::map<int, Channel*>* &channels)
{
	std::map<int, Channel*>::iterator	it;
	std::string	name = com.args[0].substr(1);

	for (it = channels->begin(); it != channels->end(); it++)
		if (it->second->getName() == name)
			return (it->second);
	
	com.sendBuffer += my_nosuchnickchannel(com.client->getNickName(), name);
	return (NULL);
}

static void	showModes(s_commands& com, std::map<int, Channel*>* &channels, bool isUser)
{
	if (com.args.size() != 1)
		return;

	if (isUser)
	{
		Client*	target = getTargetClient(com, com.args[0]);
		if (target)
			com.sendBuffer += msg_showusermodes(com, target);
		return;
	}

	Channel*	target = getTargetChannel(com, channels);
	if (target)
	{
		std::string	mode = target->getMode();
		com.sendBuffer += msg_showchannelmodes(com, target, mode);
	}
}

bool	findMode(const std::string& myModes, const char mode)
{
	if (myModes.empty())
		return (false);

	for (size_t i = 0; i < myModes.size(); i++)
		if (myModes[i] == mode)
			return (true);
	
	return (false);
}

void	Server::addUserMode(Client* &target, s_commands &com, std::string &sendBuffer, std::map<int, Channel*>* &channels)
{
	if (com.args.size() != 2 || !isSigned(com.args[1][0]))
		return;

	std::string	myMode = com.client->getMode(com.client->getChannelOfTime());
	int		currentChannel = com.client->getChannelOfTime();
	bool	iAmOperator = this->isKing(com.client->getClientFD()) || findMode(myMode, 'o');
	if (target != com.client && !iAmOperator)
	{
		com.sendBuffer += msg_err_usersdontmatch(com);
		return;
	}
	if (findMode(com.args[1], 'o') && !iAmOperator)
	{
		com.sendBuffer += msg_err_noprivileges(com);
		return;
	}
	std::string	currentMode = target->getMode(currentChannel);
	std::string	args = com.args[1].substr(1);
	char		sign = com.args[1][0];
	Channel*	channel = NULL;
	std::map<int, Channel*>::iterator	it = channels->find(currentChannel);
	channel = it->second;
	for (size_t i = 0; i < args.size(); i++)
	{
		char	mode = args[i];
		bool	isActive = findMode(currentMode, mode);

		if (mode != 'i' && mode != 'o')
		{
			com.sendBuffer += msg_err_unknownmode(com, mode);
			return;
		}
		if (mode == 'o' && !iAmOperator)
		{
			com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), channel->getName(), "You're not channel operator");
			return;
		}
		if (mode == 'i')
			target->setIsInvisible(sign == '+');
		else if (mode == 'o')
		{
			if (sign == '+' && !isActive)
				currentMode += mode;
			if (sign == '-' && isActive)
			{
				size_t	pos = currentMode.find(mode);
				currentMode.erase(pos, 1);
			}
			target->setMode(currentMode, currentChannel);
		}
	}

	if (findMode(target->getMode(currentChannel), 'o'))
	{
		target->setIsOperator(true);
		target->setOperatorChannels(channel->getName());
		channel->setOperator(target->getClientFD());
	}
	sendBuffer.clear();
	sendBuffer = msg_mode_userwelldone(com, target);
}

static void	caseT(s_commands& com, s_mode& mode)
{
	if (!mode.isKing)
	{
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}
	if (mode.sign == '+' && !mode.flagFound)
	{
		mode.currentMode += mode.flag;
		mode.target->setTopicFlag(true);
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		size_t	pos = mode.currentMode.find(mode.flag);
		mode.target->setTopicFlag(false);
		mode.currentMode.erase(pos, 1);
	}
}
static void	caseK(s_commands& com, s_mode& mode)
{
	if (!mode.isKing)
	{
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}

	bool	hasPermition = findMode(com.client->getMode(mode.channelIndex), 'o');

	if (mode.sign == '+')
	{
		if (mode.len != 3)
		{
			com.sendBuffer += msg_err_needmoreparams(com.client->getNickName(), "MODE");
			return;
		}
		if (hasPermition)
		{
			if (!mode.flagFound)
				mode.currentMode += mode.flag;
			mode.target->setPassWord(com.args[2]);
			return;
		}
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		if (hasPermition)
		{
			size_t	pos = mode.currentMode.find(mode.flag);
			mode.currentMode.erase(pos, 1);
			mode.target->getPassWord().clear();
			return;
		}
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}
}
static void	caseL(s_commands& com, s_mode& mode)
{
	if (!mode.isKing)
	{
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}
	if (mode.sign == '+')
	{
		if (mode.len != 3)
		{
			com.sendBuffer += msg_err_needmoreparams(com.client->getNickName(), "MODE");
			return;
		}
		int	limit;
		std::istringstream	ss(com.args[2]);

		ss >> limit;
		if (ss.fail())
		{
			com.sendBuffer += msg_err_invalidparameter(com, mode.target);
			return;
		}
		if (!mode.flagFound)
			mode.currentMode += mode.flag;
		mode.target->setUserLimit(limit);
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		if (mode.len != 2)
		{
			com.sendBuffer += msg_err_needmoreparams(com.client->getNickName(), "MODE");
			return;
		}
		size_t	pos = mode.currentMode.find(mode.flag);
		mode.currentMode.erase(pos, 1);
		mode.target->setUserLimit(1024);
	}
}

static void	caseO(s_commands& com, s_mode& mode)
{
	if (!mode.isKing)
	{
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}
	if (mode.len != 3)
	{
		com.sendBuffer += msg_err_needmoreparams(com.client->getNickName(), "MODE");
		return;
	}
	Client*	client = getTargetClient(com, com.args[2]);

	if (!client)
		return;
	int	clientFD = client->getClientFD();
	std::string channel = mode.target->getName();
	std::map<int,Client*>* clients = getClientsMap();
	std::map<int,Channel*>* channels = getChannelsMap();
	int	channelsIndex = getChannelsIndexMode(channel);
	int	clientsIndex = getClientsIndexMode(clientFD);

	if (mode.sign == '+')
	{
		std::string	message = std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " MODE " + "#" + channel + " +o " + (*clients)[clientFD]->getNickName() + "\r\n";
		client->setIsOperator(true);
		client->setOperatorChannels(mode.target->getName());
		if (!mode.flagFound)
			client->addMode(mode.flag, mode.channelIndex);
		mode.target->setOperator(client->getClientFD());
		mode.target->getOperatorsNames();
		newBroadcastAllChannelsMode(com, message, channel, false);
		return;
	}
	else if (mode.sign == '-')
	{
		if (channelsIndex == -1)
			return ;
		if (clientsIndex == -1)
			return ;
		std::string	message = std::string(":") + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " MODE " + "#" + channel + " -o " + (*clients)[clientFD]->getNickName() + "\r\n";
		(*channels)[channelsIndex]->getOperatorsSet().erase(clientFD);
		(*channels)[channelsIndex]->getMembersSet().insert(clientFD);
		(*clients)[clientFD]->getOperatorChannels().erase(channel);
		(*clients)[clientFD]->getChannelsSet().insert(channel);
		if ((*clients)[clientFD]->getOperatorChannels().size() == 0)
			(*clients)[clientFD]->setIsOperator(false);
		newBroadcastAllChannelsMode(com, message, channel, false);
		return ;
	}
}

static void	caseI(s_commands& com, s_mode& mode)
{
	if (!mode.isKing)
	{
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), mode.target->getName(), "You're not channel operator");
		return;
	}
	if (mode.len != 2)
	{
		com.sendBuffer += msg_err_needmoreparams(com.client->getNickName(), "MODE");
		return;
	}
	if (mode.sign == '+' && !mode.flagFound)
	{
		mode.currentMode += mode.flag;
		mode.target->setInviteFlag(true);
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		size_t	pos = mode.currentMode.find(mode.flag);
		mode.currentMode.erase(pos, 1);
		mode.target->setInviteFlag(false);
	}
}

void	Server::addChannelMode(s_commands &com, Channel* &target, int channelIndex)
{
	std::string	mode = com.client->getMode(channelIndex);

	if (target->getName() == "generic" || !findMode(mode, 'o'))
	{
		com.sendBuffer += msg_err_chanoprivsneeded(com.client->getNickName(), target->getName(), "You're not channel operator");
		return;
	}
	size_t		len = com.args.size();
	if (len < 2)
		return;

	std::string	currentMode = target->getMode();
	char		sign = com.args[1][0];
	std::string	flags = com.args[1].substr(1);
	std::string	Channel = com.args[0];
	size_t		i;
	std::map<char, void (*)(s_commands&, s_mode&)>	myMap;
	myMap['i'] = &caseI;
	myMap['o'] = &caseO;
	myMap['k'] = &caseK;
	myMap['l'] = &caseL;
	myMap['t'] = &caseT;

	std::cout << "Meu target eh o canal: " << target->getName() << std::endl;

	for (i = 0; i < flags.size(); i++)
	{
		char	flag = flags[i];
		bool	flagFound = findMode(currentMode, flag);

		if (myMap.find(flag) == myMap.end())
			continue;
		bool	k = (
			this->isKing(com.client->getClientFD())
			|| findMode(com.client->getMode(this->getChannelsIndex(target->getName())), 'o')
		);
		s_mode	mode(sign, flag, flagFound, target, currentMode, len, channelIndex, k);	
		myMap[flag](com, mode);
	}

	target->setMode(currentMode);
	com.sendBuffer += msg_showchannelmodes(com, target, flags);
	std::cout << "senha do canal: " << target->getPassWord() << std::endl;
}

void	Server::mode(s_commands &com)
{
	if (!com.args.size())
		return (callCmdMsg("Not enough parameters", 461, com, this->sendBuffer[com.index]));

	bool	isUser = com.args[0][0] != '#';

	showModes(com, this->channels, isUser);
	if (isUser)
	{
		Client*	target = getTargetClient(com, com.args[0]);
		if (!target)
			return;
		addUserMode(target, com, this->sendBuffer[com.index], this->channels);
		return;
	}
	Channel*	target = getTargetChannel(com, this->channels);
	if (!target)
		return;
	addChannelMode(com, target, getChannelsIndex(target->getName()));
}
