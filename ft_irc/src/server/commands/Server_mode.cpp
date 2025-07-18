#include "../includes/Server.hpp"
#include "../includes/messages.hpp"

// static	Client	*getTarget(s_commands &com)
// {
// 	std::map<int, Client*>::iterator	it;
// 	for (it = com.clients->begin(); it != com.clients->end(); it++)
// 		if (it->second->getNickName() == com.args[0])
// 			break;
	
// 	return (it->second);
// }

// static bool	handleNoSign(s_commands& com)
// {
// 	if (com.args.size() == 2 && (com.args[1][0] == '+' || com.args[1][0] == '-'))
// 		return (false);
// 	return (false);
// }

void	Server::mode(s_commands &com)
{
	// Client	*target;

	/*
		Handle user modes

		221 - default code for mode

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
	// target = getTarget(com);
	// if (handleNoSign(com))
	// 	return;

}
