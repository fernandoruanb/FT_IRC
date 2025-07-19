#include "../includes/Server.hpp"

void	Server::names(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called NAMES\n";
}
