#include "../includes/Server.hpp"

void	Server::invite(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called INVITE\n";
}
