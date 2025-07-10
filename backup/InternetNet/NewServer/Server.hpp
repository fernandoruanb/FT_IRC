/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:58:04 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/09 16:58:04 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

class	Server
{
	private:
		int	serverIRC;
		std::vector<pollfd>	pollFds;
		std::map<int, Client*> clients;
		std::map<std::string, Channel*> channels;
		std::string	password;
		int	port;
	public:
		Server(void);
		~Server(void);
		void	acceptNewClient(void);
		void	removeClient(int clientFD);
		void	handleInputData(int clientFD);
		void	handleOutPutData(int clientFD);
		//void	parseCommand(Client* client);
		//void	executeCommand(Client* client, std::string& cmdLine);
		void	up(void);
		void	down(void);
		void	setServerIRC(int serverIRC);
		void	setPort(int port);
		void	setPassWord(std::string password);
		int	getServerIRC(void) const;
		std::string	getPassWord(void) const;
		int	getPort(void) const;
		//Channel*	getOrCreateChannel(const std::string& name);
}
