#include "../includes/Server.hpp"

void	Server::oper(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called OPER\n";
}
