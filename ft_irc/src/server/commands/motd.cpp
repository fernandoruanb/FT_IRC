#include "../includes/Server.hpp"

void	Server::motd(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called MOTD\n";
}
