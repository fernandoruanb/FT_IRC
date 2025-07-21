#include "../includes/Server.hpp"

/*
	USER <username> <hostname> <servername> : <real name>
*/
void	Server::user(s_commands	&com)
{
	size_t	len = com.args.size();

	if (len < 3)
		return (callCmdMsg("Not enough parameters", 461, com, this->sendBuffer[com.index]));

	com.client->setUserName(com.args[0]);
	com.client->setHost(com.args[1]);
	com.client->setServerName(com.args[2]);

	/*
		fill real name

		args[3] == ':';
	*/
	if (len > 3)
	{
		std::string	name;

		for (size_t i = 4; i < len; i++)
			name += " " + com.args[i];
		
		com.client->setRealName(name);
	}
	com.client->setRegistered(true);
	this->sendBuffer[com.index] = "Hello " + com.client->getUserName() + "@" + com.client->getHost() + " " + com.client->getServerName() + " " + com.client->getRealName() + "\n";
}
