/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:02:08 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/31 20:26:35 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/messages.hpp"
#include <cstring>


// void	handleNick(std::map<int, Client*>* clients, int fd, std::string buffer, int pollIndex) {
	
// }

// bool Server::handleClientAuthentication(std::map<int, Client*>* clients, int fd, char* buffer, int pollIndex) {
// 	std::map<int, Client*>::iterator it = clients->find(fd);
// 	if (it != clients->end()) {
// 		Client* client = it->second;
// 		if (!client->getAuthenticated()) {
// 			struct pollfd (&fds)[1024] = *getMyFds();
// 			std::string input(buffer);
// 			if (input.empty() || input == "\r" || input == "\n" || input == "\r\n") {
// 				return true; // Ignore empty inputs
// 			}
// 			if (input.rfind("PASS ", 0) == 0) {
// 				std::string pass = input.substr(5);
// 				pass.erase(pass.find_last_not_of("\r\n") + 1);
// 				if (pass == this->getPassword()) {
// 					client->setAuthenticated(true);
// 					this->sendBuffer[pollIndex].clear();
// 					this->sendBuffer[pollIndex] += msg_notice("Authentication successful");
// 					this->sendBuffer[pollIndex] += msg_notice("USER <username> <hostname> <servername> :<realname>");
// 					fds[pollIndex].events |= POLLOUT;
// 					return true;
// 				} else {	
// 					this->sendBuffer[pollIndex].clear();
// 					this->sendBuffer[pollIndex] += msg_err_passwdmismatch();
// 					fds[pollIndex].events |= POLLOUT;
// 					return false;
// 				}
// 			   } else {
// 				   this->sendBuffer[pollIndex].clear();
// 				   this->sendBuffer[pollIndex] += msg_err_needmoreparams("PASS");
// 				   fds[pollIndex].events |= POLLOUT;
// 				   return false;
// 			}
// 		}
// 	}
// 	return true;
// }

// void	Server::registeredMessage(s_commands& com)
// {
// 	com.sendBuffer += msg_welcome(com.client->getNickName());
// 	com.sendBuffer += msg_yourhost(com.client->getNickName());
// 	com.sendBuffer += msg_created(com.client->getNickName());
// 	com.sendBuffer += msg_svrinfo(com.client->getNickName());
// }

void Server::tryRegister(s_commands& com)
{
    if (com.client->getAuthenticated() && com.client->hasNick() && com.client->hasUser() && !com.client->getRegistered()) {
        com.client->setRegistered(true);
        com.sendBuffer += msg_welcome(com.client);
		com.sendBuffer += msg_yourhost(com.client->getNickName());
		com.sendBuffer += msg_created(com.client->getNickName());
		com.sendBuffer += msg_svrinfo(com.client->getNickName());
    }
}

void	Server::addNewClient(int clientFD)
{
	int	index;
	struct pollfd (&fds)[1024] = getPollFds();
	std::map<int,Channel*>* channels = getChannelsMap();

	if (channels->find(0) == channels->end())
		((*channels)[0] = new Channel("generic"));
	index = 1;
	while (index < 1024)
	{
		if (fds[index].fd == -1)
			break ;
		index++;
	}
	if (index == 1024)
	{
		std::cerr << RED "Error: Maximum of FDs!!!" RESET << std::endl;
		return ;
	}
	(*this->clients)[clientFD] = new Client(clientFD);
	fds[index].fd = clientFD;
	fds[index].events = POLLIN;
	fcntl(clientFD, F_SETFL, O_NONBLOCK);
	this->numClients++;
	// NOTICE message to the new client. Asking for authentication.
	// this->sendBuffer[index] = msg_notice("Connected. Please authenticate with \"PASS <password>\"");
	fds[index].events |= POLLOUT;
	std::cout << BRIGHT_GREEN "New Client added: " << YELLOW << clientFD << RESET << std::endl;
}

Server::~Server(void)
{
	std::cout << BRIGHT_GREEN "Thank you very much for using our IRC Server =D" RESET << std::endl;
}

int	Server::atoiIRC(std::string port)
{
	const char	*tmp;
	int	result;

	tmp = port.c_str();
	result = 0;

	if (port.empty())
		return (-1);
	while (*tmp >= '0' && *tmp <= '9')
	{
		result *= 10;
		result += *tmp - '0';
		if (result > 65535)
			return (-1);
		tmp++;
	}
	if (*tmp != '\0')
		return (-1);
	return (result);
}

int	Server::getClientFDByNick(std::string nickname, int numClients)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	int	index = 1;

	while (index < numClients && fds[index].fd != -1)
	{
		if ((*clients)[fds[index].fd]->getNickName() == nickname)
			return (fds[index].fd);
		++index;
	}
	return (-1);
}

Server::Server(std::string portCheck, std::string password): supremeKey("hunter42"), supremeUser("root"), numChannels(0)
{
	int	port;

	try
	{
		this->running = getRunning();
		this->fds = getMyFds();
		this->clients = getClientsMap();
		this->channels = getChannelsMap();
		(*channels)[0] = new Channel("generic");
		numChannels++;
		//isso eh um test
		// this->createNewChannel("One", 5);
		// this->changeChannel("One", 5);

		port = atoiIRC(portCheck);
		if (port == -1)
			throw std::exception();
		if (password.empty())
			throw std::exception();
		std::cout << LIGHT_BLUE "Starting the server with your configuration 127.0.0.1:" << YELLOW << port << RESET << std::endl;
		init(port, password);

	} catch (std::exception &e) 
	{
		delete (*channels)[0];
		if (portCheck.empty())
			portCheck = "(null)";
		if (password.empty())
			password = "(null)";
		std::cerr << RED "Error: Invalid configuration (Port/Password) -> " << YELLOW << portCheck << " " << password << RESET << std::endl;
	}
}

void	Server::handleSignal(int signal)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	int	index;
	bool	*running = getRunning();

	index = 0;
	if (signal == SIGINT || signal == SIGTERM || signal == SIGQUIT)
	{
		std::cout << ORANGE "\nReceived signal to shutdown" RESET << std::endl;
		*running = false;
		while (index < 1024 && fds[index].fd != -1)
		{
			if (index > 0)
			{
				std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
				delete it->second;
			}
			close(fds[index].fd);
			fds[index].fd = -1;
			index++;
		}
		clients->clear();
		index = 0;
		std::map<int, Channel*>::iterator it = channels->find(index);
		while (it != channels->end())
		{
			if (it == channels->end())
				break ;
			delete it->second;
			it++;
		}
		channels->clear();
	}
}

void	Server::init(int port, std::string password)
{
	this->setPassword(password);
	this->setPort(port);
	std::map<int, Channel*>* channels = getChannelsMap();

	int	opt = 1;
	
	try {
		sockaddr_in	server;
		int serverIRC = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (serverIRC == -1)
			throw std::exception();

		signal(SIGINT, handleSignal);
		signal(SIGTERM, handleSignal);
		signal(SIGQUIT, handleSignal);

		this->setServerIRC(serverIRC);
		fcntl(this->serverIRC, F_SETFL, O_NONBLOCK);

		this->numClients = 1;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.s_addr = INADDR_ANY;

		setsockopt(this->serverIRC, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(serverIRC, (struct sockaddr *)&server, sizeof(server)) == 0)
		{
			std::cout << BRIGHT_GREEN "Bind successfully on " << YELLOW "127.0.0.1:" << port << RESET << std::endl;
			if (listen(serverIRC, 1024) == 0)
			{
				std::cout << LIGHT_BLUE "Listen Mode Started =D" << RESET << std::endl;
				this->startIRCService();
			}
			else
			{
				std::cerr << RED "Error: The server can't listen anything" RESET << std::endl;
				throw std::exception();
			}
		}
		else
		{
			delete (*channels)[0];
			close(serverIRC);
			std::cerr << RED "Error: Bind failed" RESET << std::endl;
			throw std::exception();
		}
	} catch (std::exception &e) {}
}

void	Server::shutdownService(void)
{
	int	index;
	struct	pollfd (&fds)[1024] = this->getPollFds();

	index = 0;
	this->setIsRunning(false);
	while (index < 1024 && fds[index].fd != -1)
	{
		close(fds[index].fd);
		fds[index].fd = -1;
		index++;
	}
}

void	Server::startIRCService(void)
{
	this->startPollFds();
	struct pollfd (&fds)[1024] = *getMyFds();
	int	ready;

	fds[0].fd = getServerIRC();
	fds[0].events = POLLIN;

	while (*this->running)
	{
		ready = poll(fds, this->getNumberOfClients(), -1);

		if (ready < 0)
		{
			if (errno == EINTR)
				continue ;
			this->shutdownService();
		}
		this->PollServerRoom();
		this->PollInputClientMonitoring();
		this->PollOutMonitoring();
	}	
}

void	Server::manageBuffers(int index)
{
	this->recvBuffer[index].clear();
	this->recvBuffer[index] = this->recvBuffer[this->numClients - 1];
	this->recvBuffer[this->numClients - 1].clear();
	this->sendBuffer[index].clear();
	this->sendBuffer[index] = this->sendBuffer[this->numClients - 1];
	this->sendBuffer[this->numClients - 1].clear();
}

int	Server::getClientsFdByName(std::string nickname)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->begin();

	while (it != clients->end())
	{
		if (nickname == it->second->getNickName())
			return (it->first);
		++it;
	}
	return (-1);
}

bool	Server::isKing(int fd) const
{
	return (this->kingsOfIRC.find(fd) != this->kingsOfIRC.end());
}
