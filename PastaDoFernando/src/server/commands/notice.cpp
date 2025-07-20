#include "../includes/Server.hpp"

void	Server::notice(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called NOTICE\n";
}
