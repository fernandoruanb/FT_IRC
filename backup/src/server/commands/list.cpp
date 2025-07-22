#include "../includes/Server.hpp"

void	Server::list(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called LIST\n";
}
