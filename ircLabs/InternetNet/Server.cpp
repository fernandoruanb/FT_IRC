/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcaldas- <fcaldas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:02:08 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/10 19:17:13 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "messages.hpp"
#include <cstring>

static std::map<int, Channel*>* getChannelsMap(void)
{
	static std::map<int, Channel*> channelsMap;

	return (&channelsMap);
}

static std::map<int, Client*>* getClientsMap(void)
{
	static std::map<int, Client*> clientsMap;

	return (&clientsMap);
}

static struct pollfd(*getMyFds(void))[1024]
{
	static struct pollfd	fds[1024];

	return (&fds);
}

bool Server::handleClientAuthentication(std::map<int, Client*>* clients, int fd, char* buffer, int pollIndex) {
	std::map<int, Client*>::iterator it = clients->find(fd);
	if (it != clients->end()) {
		Client* client = it->second;
		if (!client->getAuthenticated()) {
			struct pollfd (&fds)[1024] = *getMyFds();
			std::string input(buffer);
			if (input.empty() || input == "\r" || input == "\n" || input == "\r\n") {
				return true; // Ignore empty inputs
			}
			if (input.rfind("PASS ", 0) == 0) {
				std::string pass = input.substr(5);
				pass.erase(pass.find_last_not_of("\r\n") + 1);
				if (pass == this->getPassword()) {
					client->setAuthenticated(true);
					this->sendBuffer[pollIndex].clear();
					this->sendBuffer[pollIndex] += msg_notice("Authentication successful");
					fds[pollIndex].events |= POLLOUT;
					return true;
				} else {		
					this->sendBuffer[pollIndex].clear();
					this->sendBuffer[pollIndex] += msg_err_passwdmismatch();
					fds[pollIndex].events |= POLLOUT;
					return false;
				}
			   } else {
				   this->sendBuffer[pollIndex].clear();
				   this->sendBuffer[pollIndex] += msg_err_needmoreparams("PASS");
				   fds[pollIndex].events |= POLLOUT;
				   return false;
			}
		}
	}
	return true;
}

void	Server::addNewClient(int clientFD)
{
	int	index;
	struct pollfd (&fds)[1024] = getPollFds();
	std::map<int, Channel*>* channels = getChannelsMap();

	std::map<int, Channel*>::iterator it = channels->find(0);
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
	Client* theClient = (*this->clients)[clientFD];
	theClient->getChannelsSet().insert(0);
	theClient->setChannelOfTime(0);
	if (it != channels->end())
	{
		std::cout << LIGHT_BLUE "Adding new client " << YELLOW << clientFD << LIGHT_BLUE " To generic channel =D" << std::endl;
		Channel* generic = it->second;
		generic->addNewMember(clientFD);
	}
	fds[index].fd = clientFD;
	fds[index].events = POLLIN;
	fcntl(clientFD, F_SETFL, O_NONBLOCK);
	this->numClients++;
	// NOTICE message to the new client. Asking for authentication.
	this->sendBuffer[index] = msg_notice("Connected. Please authenticate with \"PASS <password>\"");
	fds[index].events |= POLLOUT;
	std::cout << BRIGHT_GREEN "New Client added: " << YELLOW << clientFD << RESET << std::endl;
}

void	Server::PollServerRoom(void)
{
	sockaddr_in	client;
	socklen_t	client_len;
	struct pollfd (&fds)[1024] = *getMyFds();
	int	newClientFD;

	client_len = sizeof(client);

	if (*this->running && fds[0].revents & POLLIN)
	{
		newClientFD = accept(this->serverIRC, (struct sockaddr *)&client, &client_len);
		if (newClientFD != -1) {
			this->addNewClient(newClientFD);
		}
	}
}

void	Server::PollInputClientMonitoring(void)
{
	int	index;
	ssize_t	bytes;
	char	buffer[513];
	std::map<int, Client*>* clients = getClientsMap();
	struct pollfd (&fds)[1024] = *getMyFds();

	index = 1;
	while (*this->running && index < this->numClients && fds[index].fd != -1)
	{
	   if (fds[index].revents & POLLIN)
	   {
		   bytes = recv(fds[index].fd, buffer, 512, 0);
		   if (bytes > 0) {
			   buffer[bytes] = '\0';
			   this->recvBuffer[index] += buffer; // Acumula o que chegou

			   size_t pos;
			   while ((pos = this->recvBuffer[index].find('\n')) != std::string::npos)
			   {
				   std::string line = this->recvBuffer[index].substr(0, pos + 1);
				   this->recvBuffer[index].erase(0, pos + 1);

				   if (!line.empty() && line[line.size() - 1] == '\n')
					   line.erase(line.size() - 1, 1);
				   if (!line.empty() && line[line.size() - 1] == '\r')
					   line.erase(line.size() - 1, 1);

				   std::cout << BRIGHT_GREEN "Client: " << YELLOW << fds[index].fd << LIGHT_BLUE << " " << line << RESET << std::endl;

				   if (!handleClientAuthentication(clients, fds[index].fd, (char*)line.c_str(), index)) {
					   continue;
				   }
				   Client* client = (*clients)[fds[index].fd];
				   if (!client->getAuthenticated())
				   {
					   continue;
				   }
				   if (line.rfind("PASS ", 0) == 0) {
					   continue;
				   }
				   this->sendBuffer[index].clear();
				   this->sendBuffer[index] += line;
				   this->broadcast(index);
				   //this->privmsg(index - 1, "You are very special =D\n");
				   fds[index].events |= POLLOUT;
			   }
		   }
		   if (bytes == 0)
		   {
			   std::cout << LIGHT_BLUE "Client " << YELLOW << fds[index].fd << LIGHT_BLUE " disconnected" << RESET << std::endl;
			   std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
			   delete it->second;
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
}

void	Server::PollOutMonitoring(void)
{
	int	index;
	ssize_t	bytes;
	struct pollfd (&fds)[1024] = *getMyFds();

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
static bool	*getRunning(void)
{
	static bool running = true;
	return (&running);
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
		this->clients = getClientsMap();
		this->channels = getChannelsMap();
		(*channels)[0] = new Channel("Generic");
		numChannels++;
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

void	Server::handleSignal(int signal)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	int	index;
	bool	*running = getRunning();

	index = 0;
	if (signal == SIGINT || signal == SIGTERM)
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
		index = 0;
		while (index < 1024)
		{
			std::map<int, Channel*>::iterator it = channels->find(index);

			if (it == channels->end())
				break ;
			delete it->second;
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
		server.sin_addr.s_addr = INADDR_ANY;

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
			close(serverIRC);
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

void    Server::broadcast(int sender)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->find(sender);
	if (it == clients->end())
	{
		std::cerr << RED "Error: The owner is a ghost!!!" RESET << std::endl;
		return ;
	}
	int	channelTarget = it->second->getChannelOfTime();
	int	channel;
	int    index;

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
           std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
	   if (it == clients->end())
	   {
		index++;
		continue ;
	   }
	   channel = it->second->getChannelOfTime();
           Client* client = it->second;
	   std::cout << LIGHT_BLUE "Owner channel: " << YELLOW << channelTarget << RESET << std::endl;
	   std::cout << LIGHT_BLUE "Target channel: " << YELLOW << channel << RESET << std::endl;
           if (client->getAuthenticated() && channelTarget == channel)
            {
                this->sendBuffer[index] += this->sendBuffer[sender];
                fds[index].events |= POLLOUT;
            }
            index++;
        }
        this->sendBuffer[sender].clear();
    }
}

void    Server::privmsg(int index, int sender, std::string message)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itv = clients->find(index);
	int	ownerChannel;
	int	channel;

	if (index < 1 || itv == clients->end())
		return ;
	channel = itv->second->getChannelOfTime();
	itv = clients->find(sender);
	if (itv == clients->end())
		return ;
	ownerChannel = itv->second->getChannelOfTime();
    std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
    Client* client = it->second;

    if (message.empty() || index < 0 || fds[index].fd == -1 || !client->getAuthenticated() || ownerChannel != channel)
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
