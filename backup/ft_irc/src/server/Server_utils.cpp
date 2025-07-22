#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/messages.hpp"

bool	isEmptyInput(const std::string &line)
{
	return (line.empty() || line == "\n" || line == "\r" || line == "\r\n");
}

std::map<int, Channel*>* getChannelsMap(void)
{
	static std::map<int, Channel*> channelsMap;

	return (&channelsMap);
}

std::map<int, Client*>* getClientsMap(void)
{
	static std::map<int, Client*> clientsMap;

	return (&clientsMap);
}

struct pollfd(*getMyFds(void))[1024]
{
	static struct pollfd	fds[1024];

	return (&fds);
}

bool	*getRunning(void)
{
	static bool running = true;
	return (&running);
}
