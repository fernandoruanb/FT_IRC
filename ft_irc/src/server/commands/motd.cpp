#include "../includes/Server.hpp"

void	Server::motd(s_commands& com)
{
	this->sendBuffer[com.index].clear();
	this->sendBuffer[com.index] = SERVER_NAME " 375 " + com.client->getNickName() + " :- Message of the day -\r\n";
	this->sendBuffer[com.index] += SERVER_NAME " 372 " + com.client->getNickName() + " :- Welcome to our IRC server!\r\n";
	this->sendBuffer[com.index] += SERVER_NAME " 372 " + com.client->getNickName() + " :- Respect the rules.\r\n";
	this->sendBuffer[com.index] += SERVER_NAME " 376 " + com.client->getNickName() + " :End of /MOTD command.\r\n";
}
