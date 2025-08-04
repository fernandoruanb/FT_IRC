/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:58:40 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/08/04 10:58:40 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

static std::string	getTheMessage(s_commands& com)
{
	std::size_t	index = 0;
	std::string	message;

	while (index < com.args.size() && com.args[index][0] != ':')
		++index;
	if (index >= com.args.size())
		return (message);
	message += com.args[index].substr(1);
	if (index + 1 < com.args.size())
		message += " ";
	++index;
	while (index < com.args.size())
	{
		message += com.args[index];
		if (index + 1 < com.args.size())
			message += " ";
		++index;
	}
	return (message);
}

void	Server::cap(s_commands& com)
{
	if (com.args.size() < 1)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 410 " + com.client->getNickName() + " :Invalid CAP subcommand\r\n";
		std::cerr << RED "Error: Invalid CAP subcommand" RESET << std::endl;
		return ;
	}

	std::string capabilities = getTheMessage(com);

	if (com.args[0] == "LS")
	{
		if (com.args.size() > 1 && com.args[1] == "302")
		{
			//com.client->getBufferOut() += std::string(":") + SERVER_NAME + " CAP " + com.client->getNickName() + " LS * :\r\n";
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " CAP " + com.client->getNickName() + " LS :\r\n";
			return ;
		}
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " CAP " + com.client->getNickName() + " LS :\r\n";
		return ;
	}
	else if (com.args[0] == "LIST")
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " CAP " + com.client->getNickName() +  " LIST :\r\n";
		return ;
	}
	else if (com.args[0] == "REQ")
	{
		if (capabilities.empty() || capabilities == " \r\n")
		{
			com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 410 " + com.client->getNickName() + " :Invalid CAP subcommand\r\n";
			return ;
		}
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " CAP " + com.client->getNickName() + " NAK :" + capabilities + "\r\n";
		return ;
	}
	else if (com.args[0] == "END")
		return ;
	else
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 410 " + com.client->getNickName() + " :Invalid CAP subcommand\r\n";
		return ;
	}
}
