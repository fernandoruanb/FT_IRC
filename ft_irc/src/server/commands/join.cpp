#include "../includes/Server.hpp"

void	Server::join(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called JOIN\n";
}

