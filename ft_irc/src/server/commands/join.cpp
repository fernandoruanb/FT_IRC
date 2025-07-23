#include "../includes/Server.hpp"

void	Server::join(s_commands& com)
{
	std::string	channel;

	if (com.args.size() < 1)
		return ;
	channel = com.args[0].substr(1, com.args[0].size());
	std::cout << "Olá: " << channel << std::endl;
	changeChannel(channel, com.fd);
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called JOIN\n";
}

