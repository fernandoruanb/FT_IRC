#include "../includes/Server.hpp"
#include <sstream>

static size_t	getChannelSize(Channel* &target)
{
	size_t	members = target->getMembersSet().size();
	size_t	operators = target->getOperatorsSet().size();

	return (members + operators);
}

static void	showEverybody(s_commands& com, std::map<int, Channel*>* &channels)
{
	std::map<int, Channel*>::iterator	it;

	for (it = channels->begin(); it != channels->end(); it++)
	{
		Channel*	target = it->second;

		if (!findMode(target->getMode(), 'i'))
		{
			std::stringstream ss;
			ss << getChannelSize(target);

			com.sendBuffer += msg_showtopic(target->getName(), ss.str(), target->getTopic(), com.client->getNickName());
		}
	}

	com.sendBuffer += msg_endlist(com.client->getNickName());;
}

/*
	Lista os canais, numero de usuarios e topico(se tiver)
		LIST
		#canal1 15: topico
		#canal2 2: topico
		Fim da lista
	
	Tambem podemos limitar a listar os canais visiveis
		LIST #canal1,#canal2
			#canal1 15: topico
			#canal2 2: topico
			Fim da lista
*/
void	Server::list(s_commands& com)
{
	if (com.args.size() == 0)
		return (showEverybody(com, this->channels));
	
	std::map<int, Channel*>::iterator	it;

	for (it = this->channels->begin(); it != this->channels->end(); it++)
	{
		Channel*	target = it->second;

		for (size_t i = 0; i < com.args.size(); i++)
		{
			std::string	name = com.args[i].substr(1);
			if (name == target->getName() && !findMode(target->getMode(), 'i'))
			{
				std::stringstream ss;
				ss << getChannelSize(target);

				com.sendBuffer += msg_showtopic(target->getName(), ss.str(), target->getTopic(), com.client->getNickName());
				break;
			}
		}
	}

	com.sendBuffer += msg_endlist(com.client->getNickName());
}
