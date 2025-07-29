#include "../includes/Server.hpp"
/*
	USER <username> <hostname> <servername> : <real name>
*/
void	Server::user(s_commands	&com)
{
	// if (!com.client->getAuthenticated()) {
	// 	this->sendBuffer[com.index] = my_notice_error(com.client->getNickName(), "You must authenticate first with the PASS command.");
	// 	return;
	// }
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

	/*
		fill real name

		args[3] == ':';
	*/
	if (len > 3)
	{
		std::string	name;
	
		for (size_t i = 3; i < len; i++)
			name += " " + com.args[i];
		
		com.client->setRealName(name);
		std::cout << "real name: " << com.client->getRealName() << std::endl;
	}
	if (com.client->getNickName() != "*")
		com.client->setRegistered(true);
	// if (com.client->getNickName() != "*")
	// 	com.sendBuffer += msg_welcome(com.client);
	// this->sendBuffer[com.index] = "Hello " + com.client->getUserName() + "@" + com.client->getHost() + " " + com.client->getServerName() + " " + com.client->getRealName() + "\n";
}
