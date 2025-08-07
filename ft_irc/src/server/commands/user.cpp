#include "../includes/Server.hpp"

void	Server::user(s_commands	&com)
{
	if (com.client->getRegistered()){
		this->sendBuffer[com.index] += msg_err_alreadyregistered(com.client->getNickName());
		return;
	}
	size_t	len = com.args.size();

	if (len < 3)
		return (callCmdMsg("Not enough parameters", 461, com, com.sendBuffer));

	if (com.args[0] == "*" || com.args[0] == "system")
		return (callCmdMsg("Not enough parameters", 461, com, com.sendBuffer));

	com.client->setUserName(com.args[0]);
	com.client->setHost(com.args[1]);
	com.client->setServerName(com.args[2]);
	if (len > 3 && com.args[3][0] == ':')
	{
		com.args[3] = com.args[3].substr(1);
		std::string	name;
	
		for (size_t i = 3; i < len; i++)
			name += " " + com.args[i];
		
		com.client->setRealName(name);
		std::cout << "real name: " << com.client->getRealName() << std::endl;
	}
	if (com.client->getNickName() != "*")
		com.client->setRegistered(true);
}
