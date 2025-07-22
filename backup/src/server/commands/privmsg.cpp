#include "../includes/Server.hpp"

void	Server::privmsg(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called PRIVMSG\n";
}
