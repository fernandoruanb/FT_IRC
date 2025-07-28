#include "../includes/Server.hpp"

static std::string	getTheMessage(s_commands& com)
{
	size_t	index = 0;
	std::string	message;

	while (index < com.args.size() && com.args[index][0] != ':')
		++index;
	message += com.args[index].substr(1);
	if (index + 1 < com.args.size())
		message += " ";
	while (index < com.args.size())
	{
		message += com.args[index];
		if (index + 1 < com.args.size())
			message += " ";
		++index;
	}
	return (message);
}

void	Server::kill(s_commands& com)
{
	if (com.args.size() < 1)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " KILL " + ":Not enough parameters" + "\r\n";
		return ;
	}
	//std::map<int, Client*>* clients = getClientsMap();
	std::string	nick = com.args[0];
	std::string	message = getTheMessage(com);
	int	clientFD;

	clientFD = getClientFDByNick(nick, this->numClients);
	if (clientFD == -1)
	{
		std::cerr << RED "The client doesn't exist to be KILL" RESET << std::endl;
		return ;
	}

}
