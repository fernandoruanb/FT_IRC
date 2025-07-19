#include "../includes/Server.hpp"

void	Server::pass(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called PASS\n";
}
