#include "../includes/Server.hpp"

void	Server::join(s_commands& com)
{
	std::string	channel;

	if (com.args.size() < 1)
		return ;
	if (com.args[0].empty() || com.args[0][0] != '#')
	{
		this->sendBuffer[com.index] += msg_err_nosuchchannel(com.client->getNickName(), com.args[0]);
		return ;
	}
	channel = com.args[0].substr(1, com.args[0].size());
	std::cout << "Olá: " << channel << std::endl;
	changeChannel(channel, com.fd, 0);
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called JOIN\n";
}

