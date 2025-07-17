#include "../../includes/Server.hpp"

int	Server::getServerIRC(void) const
{
	return (serverIRC);
}

struct pollfd	(&Server::getPollFds(void))[1024]
{
	return (*fds);
}

int	Server::getNumberOfClients(void) const
{
	return (numClients);
}

std::string	Server::getPassword(void) const
{
	return (password);
}

int	Server::getPort(void) const
{
	return (port);
}

int	Server::getClientsIndex(int clientFD)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	int	index;

	index = 1;
	while (index < 1024 && fds[index].fd != -1)
	{
		if (fds[index].fd == clientFD)
			return (index);
		++index;
	}
	return (-1);
}
