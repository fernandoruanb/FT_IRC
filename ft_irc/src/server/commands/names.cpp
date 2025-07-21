#include "../includes/Server.hpp"

/*
	List all users in a channel

	if use NAMES - no arguments
		shows all user in public channels
	
	
*/
void	Server::names(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = "You called NAMES\n";
}
