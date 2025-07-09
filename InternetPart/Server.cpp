/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@42sp.org.br>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:02:08 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/08 19:09:33 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static bool	*getRunning(void)
{
	static bool running = true;
	return (&running);
}

static struct pollfd(*getMyFds(void))[1024]
{
	static struct pollfd	fds[1024];

	return (&fds);
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

Server::Server(std::string portCheck, std::string password)
{
	int	port;

	try
	{
		this->running = getRunning();
		this->fds = getMyFds();
		port = atoiIRC(portCheck);
		if (port == -1)
			throw std::exception();
		if (password.empty())
			throw std::exception();
		std::cout << LIGHT_BLUE "Starting the server with your configuration 127.0.0.1:" << YELLOW <<  port << RESET << std::endl;
		init(port, password);

	} catch (std::exception &e) 
	{
		if (portCheck.empty())
			portCheck = "(null)";
		if (password.empty())
			password = "(null)";
		std::cerr << RED "Error: Invalid configuration (Port/Password) -> " << YELLOW << portCheck << " " << password << RESET << std::endl;
	}
}

std::string	Server::getPassword(void) const
{
	return (password);
}

int	Server::getPort(void) const
{
	return (port);
}

void	Server::setPort(int port)
{
	this->port = port;
}

void	Server::setPassword(std::string password)
{
	this->password = password;
}

void	Server::setIsRunning(bool signal)
{
	*this->running = signal;
}

void	Server::setServerIRC(int serverFD)
{
	this->serverIRC = serverFD;
}

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

void	Server::handleSignal(int signal)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	int	index;
	bool	*running = getRunning();

	index = 0;
	if (signal == SIGINT || signal == SIGTERM)
	{
		std::cout << ORANGE "Received signal to shutdown" RESET << std::endl;
		*running = false;
		while (index < 1024 && fds[index].fd != -1)
		{
			close(fds[index].fd);
			fds[index].fd = -1;
			index++;
		}
	}
}

void	Server::init(int port, std::string password)
{
	this->setPassword(password);
	this->setPort(port);

	int	opt = 1;
	
	try {
		sockaddr_in	server;
		int serverIRC = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (serverIRC == -1)
			throw std::exception();

		signal(SIGINT, handleSignal);
		signal(SIGTERM, handleSignal);

		this->setServerIRC(serverIRC);
		fcntl(this->serverIRC, F_SETFL, O_NONBLOCK);

		this->numClients = 1;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.s_addr = inet_addr("127.0.0.1");

		setsockopt(this->serverIRC, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(serverIRC, (struct sockaddr *)&server, sizeof(server)) == 0)
		{
			std::cout << BRIGHT_GREEN "Bind successfully on " << YELLOW "127.0.0.1:" << port << RESET << std::endl;
			if (listen(serverIRC, 1024) == 0)
			{
				std::cout << LIGHT_BLUE "Listen Mode Started =D" << std::endl;
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
			std::cerr << RED "Error: Bind failed" RESET << std::endl;
			throw std::exception();
		}
	} catch (std::exception &e) {}
}

void	Server::startPollFds(void)
{
	int	index;
	struct pollfd	(&fds)[1024] = this->getPollFds();

	index = 0;
	while (index < 1024)
	{
		fds[index].fd = -1;
		index++;
	}
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

void	Server::addNewClient(int clientFD)
{
	int	index;
	struct pollfd (&fds)[1024] = getPollFds();

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
	fds[index].fd = clientFD;
	fds[index].events = POLLIN;
	fcntl(clientFD, F_SETFL, O_NONBLOCK);
	this->numClients++;
	std::cout << BRIGHT_GREEN "New Client added: " << YELLOW << clientFD << RESET << std::endl;
}

void	Server::startIRCService(void)
{
	sockaddr_in	client;
	socklen_t	client_len;
	char	buffer[513];
	this->startPollFds();
	struct pollfd (&fds)[1024] = *getMyFds();
	int	ready;
	int	newClient;
	int	index;
	ssize_t	bytes;

	fds[0].fd = getServerIRC();
	fds[0].events = POLLIN;
	client_len = sizeof(client);

	while (*this->running)
	{
		ready = poll(fds, this->getNumberOfClients(), -1);

		if (ready < 0)
		{
			if (errno == EINTR)
				continue ;
			this->shutdownService();
		}
		if (*this->running && fds[0].revents & POLLIN)
		{
			newClient = accept(this->serverIRC, (struct sockaddr *)&client, &client_len);
			if (newClient != -1)
				this->addNewClient(newClient);
		}
		index = 0;
		while (*this->running && index < this->numClients && fds[index].fd != -1)
		{
			if (fds[index].revents & POLLIN)
			{
				bytes = recv(fds[index].fd, buffer, 512, 0);
				if (bytes > 0)
				{
					buffer[bytes] = '\0';
					std::cout << BRIGHT_GREEN "Client: " << YELLOW << fds[index].fd << LIGHT_BLUE << " " << buffer << RESET << std::endl;
					this->recvBuffer[index] = buffer;
					this->sendBuffer[index].clear();
					this->sendBuffer[index] += buffer;
					this->broadcast(index);
                                	this->privmsg(index - 1, "You are very special =D\n");
					fds[index].events |= POLLOUT;
				}
				if (bytes == 0)
				{
					std::cout << LIGHT_BLUE "Client " << YELLOW << fds[index].fd << LIGHT_BLUE " disconnected" << RESET << std::endl;
					close(fds[index].fd);
					fds[index].fd = fds[numClients - 1].fd;
					fds[numClients - 1].fd = -1;
					fds[numClients - 1].events = 0;
					this->manageBuffers(index);
					this->numClients--;
				}
			}
			index++;
		}
		index = 0;
		while (*this->running && index < this->numClients && fds[index].fd != -1)
		{
			if (fds[index].revents & POLLOUT)
			{
				bytes = send(fds[index].fd, this->sendBuffer[index].c_str(), sendBuffer[index].size(), 0);
				if (bytes > 0)
					this->sendBuffer[index].erase(0, bytes);
				if (this->sendBuffer[index].empty())
					fds[index].events &= ~POLLOUT;
			}
			index++;
		}
	}	
}

void	Server::broadcast(int sender)
{
	struct pollfd (&fds)[1024] = getPollFds();
	int	index;

	index = 1;
	if (!this->sendBuffer[sender].empty())
	{
		while (index < this->numClients && fds[index].fd != -1)
		{
			if (index == sender)
			{
				index++;
				continue ;
			}
			this->sendBuffer[index] = this->sendBuffer[sender];
			fds[index].events |= POLLOUT;
			index++;
		}
		this->sendBuffer[sender].clear();
	}
}

void	Server::privmsg(int index, std::string message)
{
	struct pollfd (&fds)[1024] = getPollFds();

	if (message.empty() || index < 0 || fds[index].fd == -1)
		return ;
	this->sendBuffer[index] += message;
	fds[index].events |= POLLOUT;
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
