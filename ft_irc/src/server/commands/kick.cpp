#include "../includes/Server.hpp"

/*static int	getTheTargetFD(const std::string target, int numClients)
{
	std::map<int, Client*>* clients = getClientsMap();

	struct pollfd (&fds)[1024] = *getMyFds();
	int	index = 1;
	int	targetFD = -1;

	while (index < numClients && fds[index].fd != -1)
	{
		if ((*clients)[fds[index].fd]->getNickName() == target)
		{
			targetFD = fds[index].fd;
			return (targetFD);
		}
		++index;
	}
	return (targetFD);
}*/

static std::vector<std::string>	getAllClients(s_commands& com)
{
	std::vector<std::string>	clients;
	std::size_t	index = 0;

	while (index < com.args.size())
	{
		if (com.args[index][0] == ':')
			break ;
		if (com.args[index][0] != '#')
		{
			if (std::find(clients.begin(), clients.end(), com.args[index]) == clients.end())
				clients.push_back(com.args[index]);
		}
		++index;
	}
	return (clients);
}

static std::string	getTheMessage(s_commands& com)
{
	std::size_t	index = 0;
	std::string	message;

	while (index < com.args.size())
	{
		if (com.args[index][0] == ':')
		{
			message += com.args[index].substr(1);
			message += " ";
			++index;
			while (index < com.args.size())
			{
				message += com.args[index];
				if (index + 1 < com.args.size())
					message += " ";
				++index;
			}
			message += "\r\n";
			return (message);
		}
		++index;
	}
	return (message);
}

void	Server::kick(s_commands& com)
{
	std::vector<std::string>	clientsVector = getAllClients(com);
	int	targetFD = -1;
	std::string	message = getTheMessage(com);
	std::size_t	index;

	if (com.args.size() < 2) {
		this->sendBuffer[com.index] += msg_err_needmoreparams("KICK");
		return ;
	}

	if (clientsVector.empty())
	{
		this->sendBuffer[com.index] += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " KICK " + ":Not enough parameters" + "\r\n";
		return ;
	}
	if (message.empty() || message == " \r\n")
		message = "You were kicked\r\n";
	index = 0;
	while (index < clientsVector.size() && index < 1024 && !clientsVector[index].empty())
	{
		targetFD = getClientsFdByName(clientsVector[index]);
		if (targetFD == -1)
		{
			std::cerr << RED "Error: the client doesn't exist to kick" RESET << std::endl;
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 401 " + com.client->getNickName() + " " + clientsVector[index] + " :No such nick/channel" + "\r\n";
			++index;
			continue ;
		}
		this->kickFromChannel(com.args[0].substr(1), com.fd, targetFD, message);
		++index;
	}
}
