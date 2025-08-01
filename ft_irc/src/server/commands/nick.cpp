#include "../includes/Server.hpp"
#include "../includes/messages.hpp"

static bool	checkChar(char c)
{
	bool	isLetter = std::isalpha(static_cast<unsigned char>(c));
	bool	isDigit = std::isdigit(static_cast<unsigned char>(c));
	bool	isSpecial = c == '-'
			|| c == '['
			|| c == ']'
			|| c == '\\'
			|| c == '^'
			|| c == '{'
			|| c == '}'
			|| c == '`';

	return (isLetter || isDigit || isSpecial);
}

static bool	validChar(const std::string& name)
{
	if (name.empty()
		|| !std::isalpha(static_cast<unsigned char>(name[0])))
		return (false);
	for (size_t i = 0; i < name.size(); i++)
		if (!checkChar(name[i]))
			return (false);

	return(true);
}

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
	std::string	oldNick = com.client->getNickName();

	if (!com.args.size())
	{
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] = msg_error("No nickname given", 431, com);
		return;
	}
	if (com.args.size() == 1)
	{
		if (!validChar(com.args[0]))
		{
			com.sendBuffer += msg_err_erroneusnickname(com.args[0]);
			return;
		}
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
		// this->tryRegister(com);
	}
	this->sendBuffer[com.index] = msg_notice("NICK " + com.client->getNickName());
	if (!com.client->getRegistered() && com.client->getUserName() != "*")
	{
		com.client->setRegistered(true);
		// Chamar tryRegister para garantir que as mensagens de boas-vindas sejam enviadas
		this->tryRegister(com);
	}
	
	std::string	message =	oldNick
								+ "!"
								+ com.client->getUserName()
								+ "@"
								+ com.client->getHost()
								+ " NICK :"
								+ com.client->getNickName()
								+ "\n";
	messageToAllChannels(com, message);
	
}
