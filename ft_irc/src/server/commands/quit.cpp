#include "../includes/Server.hpp"

void	Server::desconect(s_commands& com)
{
	struct pollfd (&fds)[1024] = *getMyFds();

	delete com.client;
	close(fds[com.index].fd);
	fds[com.index].fd = fds[this->numClients - 1].fd;
	fds[this->numClients - 1].fd = -1;
	fds[this->numClients - 1].events = 0;
	--this->numClients;
	com.isOnline = false;
}

void	Server::messageToAllChannels(s_commands& com, const std::string& message)
{
	/*
		Vms mandar a mensagem para tds os canais do com.client

		esse std::set<std::string> tem os nomes dos canais, vms procurar eles e mandar broadcast
			com.client->getChannels();
			broadcast(com.index, message, <index do canal>);
	*/
}

void	Server::quit(s_commands& com)
{
	if (com.args.size())
	{
		std::string	message;

		for (size_t i = 0; i < com.args.size(); i++)
			message += com.args[i] + ' ';
		
		messageToAllChannels(com, message)
	}
	desconect(com);
}
