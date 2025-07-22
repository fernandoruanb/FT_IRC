#include "../includes/Server.hpp"

void	Server::kick(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called KICK\n";
}
