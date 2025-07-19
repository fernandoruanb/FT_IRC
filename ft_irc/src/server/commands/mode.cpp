#include "../includes/Server.hpp"

static bool	isSigned(const char c)
{
	return (c == '+' || c == '-');
}

static	Client	*getTarget(s_commands &com)
{
	std::map<int, Client*>::iterator	it;

	for (it = com.clients->begin(); it != com.clients->end(); it++)
		if (it->second->getNickName() == com.args[0])
			return (it->second);
	
	return (NULL);
}

static void	showModes(Client*	&target, s_commands& com, std::string &sendBuffer)
{
	if (com.args.size() != 1)
		return;
	sendBuffer.clear();
	sendBuffer = msg_error(target->getMode(), 221);
}

static void	addUserMode(Client* &target, s_commands &com, std::string &sendBuffer)
{
	std::string	currentMode;

	if (com.args.size() > 2 || com.args.size() < 2 || !isSigned(com.args[1][0]))
		return;
	
	currentMode = target->getMode();
	if (com.args[1][0] == '+')
		currentMode += &com.args[1][1];

	target->setMode(currentMode);
	sendBuffer.clear();
	sendBuffer = msg_notice("MODE " + target->getNickName() + " " + com.args[1]);
}

void	Server::mode(s_commands &com)
{
	Client	*target;

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
	if (!com.args.size())
	{
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] = msg_error("Not enough parameters", 461);
		return;
	}

	target = getTarget(com);
	showModes(target, com, this->sendBuffer[com.index]);
	addUserMode(target, com, this->sendBuffer[com.index]);

}
