#include "../includes/Server.hpp"
#include "../includes/messages.hpp"

static bool	validNick(s_commands& com)
{
	std::map<int, Client*>::iterator	it;
	for (it = com.clients->begin(); it != com.clients->end(); it++)
		if (it->second->getNickName() == com.args[0])
			return (false);
	return (true);
}

void	Server::nick(s_commands& com)
{
	if (!com.args.size())
	{
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] = msg_error("ERR_NONICKNAMEGIVEN", 431);
		return;
	}
	if (com.args.size() == 1)
	{
		if (validNick(com))
			com.client->setNickName(com.args[0]);
		else
		{
			this->sendBuffer[com.index].clear();
			this->sendBuffer[com.index] = msg_error("ERR_NICKNAMEINUSE", 433);
		}
		return;
	}
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = msg_notice("NICK <your nickname>");
}
