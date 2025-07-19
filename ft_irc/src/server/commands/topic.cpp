#include "../includes/Server.hpp"

void	Server::topic(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called TOPIC\n";
}
