#include "../includes/Server.hpp"
#include "../includes/messages.hpp"

bool	validNick(s_commands& com)
{
	if (com.args[0] == "*" || com.args[0] == "system")
		return (false);

	std::map<int, Client*>::iterator	it;
	for (it = com.clients->begin(); it != com.clients->end(); it++)
		if (it->second->getNickName() == com.args[0])
			return (false);
	return (true);
}

/*
	ainda falta
		-mensagem para tds os canais q esse user esta
		:antigoNick!user@host NICK :novoNick
*/
void	Server::nick(s_commands& com)
{
	// if (!com.client->getAuthenticated()) {
	// 	this->sendBuffer[com.index] = my_notice_error(com.client->getNickName(), "You must authenticate first with the PASS command.");
	// 	return;
	// }
	if (!com.args.size())
	{
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] = msg_error("No nickname given", 431, com);
		return;
	}
	if (com.args.size() == 1)
	{
		if (validNick(com))
			com.client->setNickName(com.args[0]);
		else
		{
			this->sendBuffer[com.index].clear();
			this->sendBuffer[com.index] = msg_error("Nickname is already in use", 433, com);
			return;
		}
		// com.sendBuffer.clear();
		// com.sendBuffer = "Hello " + com.client->getNickName() + "\n";

		this->tryRegister(com);
		return;
	}
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = msg_notice("NICK <your nickname>");
}
