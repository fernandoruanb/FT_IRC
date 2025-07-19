#include "../includes/Server.hpp"

void	Server::mode(s_commands &com)
{
	// this->sendBuffer[com.index].clear();
	// this->sendBuffer[com.index] += msg_notice("Mode was called");
	std::cout << "Chegamos no mode e esse eh o send buffer: " << this->sendBuffer[com.index] << std::endl;
}
