#include "../includes/Server.hpp"

void	Server::oper(s_commands& com)
{
	if (com.args.size() != 2)
	{
		if (com.args.size() < 2)
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " OPER " + ":Not enough parameters" + "\r\n";
		else
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " OPER " + ":Too many parameters" + "\r\n";
		return ;
	}
	std::string user = com.args[0];
	std::string password = com.args[1];

	if (user == this->supremeUser && password == this->supremeKey)
	{
		std::cout << LIGHT_BLUE "The client " << YELLOW << com.client->getNickName() << LIGHT_BLUE " has been promoted to a new " << ORANGE << " KING " << LIGHT_BLUE " of our IRC Server" RESET << std::endl;
		this->kingsOfIRC.insert(com.fd);
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 381 " + com.client->getNickName() + " :You are a new IRC operator" + "\r\n";
	}
	else
	{
		std::cerr << RED "The client " << YELLOW << com.client->getNickName() << RED " is trying to become a new " << ORANGE << "King " << LIGHT_BLUE " of our IRC server" RESET << std::endl;
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 464 " + com.client->getNickName() + " :User/Password incorrect" + "\r\n";
	}
}
