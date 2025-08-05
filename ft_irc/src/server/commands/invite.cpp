#include "../includes/Server.hpp"

void	Server::invite(s_commands& com)
{
	if (com.args.size() < 2)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " INVITE :Not enough parameters\r\n";
		return ;
	}
	if (com.args[1][0] != '#')
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + com.args[1] + " :No such nick/channel\r\n";
		return ;
	}

	std::string	client;
	std::string	channel = com.args[1].substr(1, com.args[1].size());

	client = com.args[0];
	int	clientFD = getClientsFdByName(client);
	if (clientFD == -1)
	{
		com.client->getBufferOut() += my_nosuchnickchannel(com.client->getNickName(), client);
		return ;
	}
	int	channelIndex = getChannelsIndex(com.args[1].substr(1));
	if (channelIndex == -1)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 403 " + com.client->getNickName() + " " + com.args[1] + " :No such nick/channel\r\n";
		return ;
	}
	inviteToChannel(channel, com.fd, clientFD);
}
