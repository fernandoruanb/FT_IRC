#include "../includes/Server.hpp"

void	Server::kill(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called KILL\n";
}
