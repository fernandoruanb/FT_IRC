#include "../includes/Server.hpp"
#include <sstream>

s_mode::s_mode(char s, char f, bool ff, Channel* &t, std::string& c, size_t l, int i, bool k)
	: sign(s), flag(f), flagFound(ff), target(t), currentMode(c), len(l), channelIndex(i), isKing(k)
{}

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
		com.sendBuffer += msg_showchannelmodes(com, target);
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

		if (sign == '+' && !isActive)
			currentMode += mode;
		if (sign == '-' && isActive)
		{
			size_t	pos = currentMode.find(mode);
			currentMode.erase(pos, 1);
		}
	}

	target->setMode(currentMode, currentChannel);
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
	if (mode.sign == '+')
	{
		client->setIsOperator(true);
		client->setOperatorChannels(mode.target->getName());
		if (!mode.flagFound)
			client->addMode(mode.flag, mode.channelIndex);
		mode.target->setOperator(client->getClientFD());
		mode.target->getOperatorsNames();
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		size_t	pos = mode.currentMode.find(mode.flag);

		mode.currentMode.erase(pos, 1);
		client->setIsOperator(false);

		std::string	name = mode.target->getName();
		std::set<std::string>::iterator cpos = client->getOperatorChannels().find(name);
		client->getOperatorChannels().erase(cpos);
		
		std::set<int>::iterator	tpos = mode.target->getOperatorsSet().find(com.client->getClientFD());
		mode.target->getOperatorsSet().erase(tpos);
		mode.target->getOperatorsNames();
		mode.target->addNewMember(client->getClientFD());
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

	if (target->getName() == "Generic" || !findMode(mode, 'o'))
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
	com.sendBuffer += msg_showchannelmodes(com, target);
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
