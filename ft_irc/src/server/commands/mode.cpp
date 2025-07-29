#include "../includes/Server.hpp"
#include <sstream>

s_mode::s_mode(char s, char f, bool ff, Channel* &t, std::string& c, size_t l, int i)
	: sign(s), flag(f), flagFound(ff), target(t), currentMode(c), len(l), channelIndex(i)
{}

static bool	isSigned(const char c)
{
	return (c == '+' || c == '-');
}

static	Client	*getTargetClient(s_commands &com, const std::string& name, std::string &sendBuffer)
{
	std::map<int, Client*>::iterator	it;

	for (it = com.clients->begin(); it != com.clients->end(); it++)
		if (it->second->getNickName() == name)
			return (it->second);
	
	callCmdMsg("No such nick/channel", 401, com, sendBuffer);
	return (NULL);
}

static Channel*	getTargetChannel(s_commands &com, std::map<int, Channel*>* &channels, std::string &sendBuffer)
{
	std::map<int, Channel*>::iterator	it;
	//	Removing # from #<chanelName>
	std::string	name = com.args[0].substr(1);

	for (it = channels->begin(); it != channels->end(); it++)
		if (it->second->getName() == name)
			return (it->second);
	
	callCmdMsg("No such nick/channel", 401, com, sendBuffer);
	return (NULL);
}

static void	showModes(s_commands& com, std::string &sendBuffer, std::map<int, Channel*>* &channels, bool isUser)
{
	if (com.args.size() != 1)
		return;

	if (isUser)
	{
		Client*	target = getTargetClient(com, com.args[0], sendBuffer);
		if (target)
			callCmdMsg("+" + target->getMode(com.client->getChannelOfTime()), 221, com, sendBuffer);
		return;
	}

	Channel*	target = getTargetChannel(com, channels, sendBuffer);
	if (target)
		callCmdMsg("+" + target->getMode(), 221, com, sendBuffer);

}

/*
	Check if (char)mode is in myModes

	how to use

		findMode(getMode(), 'o');
*/
bool	findMode(const std::string& myModes, const char mode)
{
	if (myModes.empty())
		return (false);

	for (size_t i = 0; i < myModes.size(); i++)
		if (myModes[i] == mode)
			return (true);
	
	return (false);
}

static void	addUserMode(Client* &target, s_commands &com, std::string &sendBuffer, std::map<int, Channel*>* &channels)
{
	if (com.args.size() != 2 || !isSigned(com.args[1][0]))
		return;
	
	//vc so pode alterar o seu proprio mode c vc n for operador
	std::string	myMode = com.client->getMode(com.client->getChannelOfTime());
	bool	iAmOperator = findMode(myMode, 'o');
	if (target != com.client && !iAmOperator)
	{
		com.sendBuffer += "N vai fazer isso n malandro\n";
		return;
	}
	(void)channels;
	std::string	currentMode = target->getMode(com.client->getChannelOfTime());
	std::string	args = com.args[1].substr(1);
	char		sign = com.args[1][0];

	//	Check the target modes
	for (size_t i = 0; i < args.size(); i++)
	{
		char	mode = args[i];
		bool	isActive = findMode(currentMode, mode);

		if (mode != 'i' && mode != 'o')
		{
			std::cout << "Mode: " << mode << std::endl;
			com.sendBuffer += "Faz denovo, faz direito. Esse modo n vale aqui\n";
			return;
		}
		if (mode == 'o' && !iAmOperator)
		{
			std::cout << "achei? " << iAmOperator << std::endl;
			std::cout << "meu modo: " + com.client->getMode(com.client->getChannelOfTime()) << std::endl;
			callCmdMsg("You're not channel operator", 482, com, com.sendBuffer);
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

	target->setMode(currentMode, com.client->getChannelOfTime());
	if (findMode(target->getMode(com.client->getChannelOfTime()), 'o'))
	{
		//tem q chamar a setOperator do canal aqui dps
		target->setIsOperator(true);
		// Channel*	channel = NULL;
		// std::map<int, Channel*>::iterator	it = channels->find(com.client->getChannelOfTime());
		
	}
	sendBuffer.clear();
	sendBuffer = msg_notice("MODE " + target->getNickName() + " " + com.args[1]);
	// std::cout << target->getNickName() << " Is operator: " << target->getIsOperator() << std::endl;
}

static void	caseT(s_commands& com, s_mode& mode)
{
	(void)com;
	if (mode.sign == '+' && !mode.flagFound)
	{
		mode.currentMode += mode.flag;
		mode.target->setTopicFlag(true);
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		size_t	pos = mode.currentMode.find(mode.flag);
		mode.currentMode.erase(pos, 1);
	}
}
static void	caseK(s_commands& com, s_mode& mode)
{
	bool	hasPermition = findMode(com.client->getMode(mode.channelIndex), 'o');

	if (mode.sign == '+')
	{
		if (mode.len != 3)
		{
			com.sendBuffer += "Invalid num of arguments\n";
			return;
		}
		std::cout << "indice do canal: " << mode.channelIndex << std::endl;
		if (hasPermition)
		{
			if (!mode.flagFound)
				mode.currentMode += mode.flag;
			mode.target->setPassWord(com.args[2]);
			return;
		}
		com.sendBuffer += "vc n tem permissao pra isso aqui n amigo'-'\n";
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
		com.sendBuffer += "vc n tem permissao pra isso aqui n amigo'-'\n";
		return;
	}
}
static void	caseL(s_commands& com, s_mode& mode)
{
	if (mode.sign == '+' && !mode.flagFound)
	{
		if (mode.len != 3)
		{
			com.sendBuffer += "Invalid num of arguments\n";
			return;
		}
		int	limit;
		std::istringstream	ss(com.args[2]);

		ss >> limit;
		if (ss.fail())
		{
			com.sendBuffer += "Tem coisa errada no numero de clientes q vc pos\n";
			return;
		}
		mode.currentMode += mode.flag;
		mode.target->setUserLimit(limit);
		return;
	}
	if (mode.sign == '-' && mode.flagFound)
	{
		if (mode.len != 2)
		{
			com.sendBuffer += "Invalid num of arguments\n";
			return;
		}
		size_t	pos = mode.currentMode.find(mode.flag);
		mode.currentMode.erase(pos, 1);
		mode.target->setUserLimit(1024);
	}
}
static void	caseO(s_commands& com, s_mode& mode)
{
	if (mode.len != 3)
	{
		com.sendBuffer += "Invalid num of arguments\n";
		return;
	}
	Client*	client = getTargetClient(com, com.args[2], com.sendBuffer);
	if (!client)
		return;
	client->setIsOperator(true);
	client->addMode(mode.flag, mode.channelIndex);
	mode.target->setOperator(client->getClientFD());
}

static void	caseI(s_commands& com, s_mode& mode)
{
	if (mode.len != 2)
	{
		com.sendBuffer += "Invalid num of arguments\n";
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

static void	addChannelMode(s_commands &com, Channel* &target, int channelIndex)
{
	std::string	mode = com.client->getMode(channelIndex);

	if (target->getName() == "Generic" || !findMode(mode, 'o'))
	{
		com.sendBuffer += "Pode nao man ;-;\n";
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
		
		s_mode	mode(sign, flag, flagFound, target, currentMode, len, channelIndex);	
		myMap[flag](com, mode);
	}

	target->setMode(currentMode);
	com.sendBuffer += msg_notice("MODE " + target->getName() + " " + com.args[1]);
	std::cout << "senha do canal: " << target->getPassWord() << std::endl;
}
/*
	Handle user modes

	221 - default code for mode
	mode var default value = "+"

	//	Show channel/user modes
	MODE <nick/#channel> || MODE <target> i(no sign flag)
	output:
		- No modes
			:irc.example.com 221 Miku +
		-With modes
			:irc.example.com 221 Miku +i

	MODE <nick/#channel> +/- io
	channel must have the # before name

	flags
	i(user) - invisible to WHO and NAMES						-	MODE <nick> +/-i
	i(channel) - only invite channel
	o - OPERATOR privilege to one user of the channel	-	MODE #channel +/-o <nick> || MODE <nick> +/-o
	t - only operators can change the topic				-	MODE #channel +/-t
	k - asks for password to enter the channel			-	MODE #channel +/-k <passwd>
	l - limit the users amount in the channel			-	MODE #channel +/-l <amount>
*/
void	Server::mode(s_commands &com)
{
	if (!com.args.size())
		return (callCmdMsg("Not enough parameters", 461, com, this->sendBuffer[com.index]));

	bool	isUser = com.args[0][0] != '#';

	showModes(com, this->sendBuffer[com.index], this->channels, isUser);
	if (isUser)
	{
		Client*	target = getTargetClient(com, com.args[0], this->sendBuffer[com.index]);
		if (!target)
			return;
		addUserMode(target, com, this->sendBuffer[com.index], this->channels);
		return;
	}
	Channel*	target = getTargetChannel(com, this->channels, com.sendBuffer);
	if (!target)
		return;
	addChannelMode(com, target, getChannelsIndex(target->getName()));
}
