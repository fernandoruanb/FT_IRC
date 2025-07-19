#include "../includes/Server.hpp"

void	Server::quit(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called QUIT\n";
}
