#include "../includes/Server.hpp"

void	Server::privmsg(s_commands& com)
{
	if (com.args.size() < 2)
		return ;
	if (com.args[1][0] != ':')
		return ;
	std::string	message = com.args[1].substr(1, com.args[1].size());
	std::string	target = com.args[0];

	std::cout << "Tô aqui" << std::endl;
	if (com.args[0][0] == '#')
	{
		broadcast(com.index, message);
		return ;
	}
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called PRIVMSG\n";
}
