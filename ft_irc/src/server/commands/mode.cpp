#include "../includes/Server.hpp"

static bool	isSigned(const char c)
{
	return (c == '+' || c == '-');
}

static	Client	*getTargetClient(s_commands &com, std::string &sendBuffer)
{
	std::map<int, Client*>::iterator	it;

	for (it = com.clients->begin(); it != com.clients->end(); it++)
		if (it->second->getNickName() == com.args[0])
			return (it->second);
	
	callCmdMsg("No such nick/channel", 401, com, sendBuffer);
	return (NULL);
}

static Channel*	getTargetChannel(s_commands &com, std::map<int, Channel*>* &channels, std::string &sendBuffer)
{
	std::map<int, Channel*>::iterator	it;
	//	Removing # from #Generic
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
		Client*	target = getTargetClient(com, sendBuffer);
		if (target)
			callCmdMsg(target->getMode(), 221, com, sendBuffer);
		return;
	}

	Channel*	target = getTargetChannel(com, channels, sendBuffer);
	if (target)
		callCmdMsg(target->getMode(), 221, com, sendBuffer);

}

/*
	Check if (char)mode is in myModes

	how to use

		findMode(getMode(), 'o');
*/
bool	findMode(const std::string& myModes, const char mode)
{
	if (myModes.empty() || myModes.size() < 2)
		return (false);

	for (size_t i = 1; i < myModes.size(); i++)
		if (myModes[i] == mode)
			return (true);
	
	return (false);
}

static void	addUserMode(Client* &target, s_commands &com, std::string &sendBuffer)
{
	if (com.args.size() != 2 || !isSigned(com.args[1][0]))
		return;
	
	std::string	currentMode = target->getMode();
	std::string	args = com.args[1].substr(1);
	char		sign = com.args[1][0];

	if (currentMode.empty() || currentMode[0] != '+')
		currentMode.insert(0, 1, '+');

	//	Check the target modes
	for (size_t i = 0; i < args.size(); i++)
	{
		bool	isActive = false;
		char	mode = args[i];

		for (size_t j = 0; j < currentMode.size(); j++)
			if (currentMode[j] == mode)
			{
				isActive = true;
				break;
			}
		
		if (sign == '+' && !isActive)
			currentMode += mode;
		if (sign == '-' && isActive)
		{
			size_t	pos = currentMode.find(mode);
			currentMode.erase(pos, 1);
		}
	}

	target->setMode(currentMode);
	target->setIsOperator(findMode(target->getMode(), 'o'));
	sendBuffer.clear();
	sendBuffer = msg_notice("MODE " + target->getNickName() + " " + com.args[1]);
	// std::cout << target->getNickName() << " Is operator: " << target->getIsOperator() << std::endl;
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
	i - invisible to WHO and NAMES						-	MODE <nick> +/-i
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
		Client*	target = getTargetClient(com, this->sendBuffer[com.index]);
		if (!target)
			return;
		addUserMode(target, com, this->sendBuffer[com.index]);
	}
	//Tratar os canais qnd fernando subir o update

}
