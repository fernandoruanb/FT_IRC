#include "../includes/Server.hpp"

void	Server::part(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called PART\n";
}
