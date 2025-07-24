#include "../includes/Server.hpp"

void	Server::invite(s_commands& com)
{
	if (com.args.size() < 2)
		return ;
	if (com.args[1][0] != '#')
		return ;

	std::string	client;
	std::string	channel = com.args[1].substr(1, com.args[1].size());

	client = com.args[0];
	int	clientFD = getClientsFdByName(client);
	inviteToChannel(channel, com.fd, clientFD);
}
