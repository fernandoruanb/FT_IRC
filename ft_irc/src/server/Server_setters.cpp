#include "../../includes/Server.hpp"

void	Server::setPort(int port)
{
	this->port = port;
}

void	Server::setPassword(std::string password)
{
	this->password = password;
}

void	Server::setIsRunning(bool signal)
{
	*this->running = signal;
}

void	Server::setServerIRC(int serverFD)
{
	this->serverIRC = serverFD;
}
void	Server::setCurrentCommand(s_commands &com)
{
	this->currentCommand = &com;
}