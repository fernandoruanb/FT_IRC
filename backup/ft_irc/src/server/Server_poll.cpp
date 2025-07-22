#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/messages.hpp"

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

static bool	checkSignUp(Client* &client, std::string &sendBuffer, int i)
{
	struct pollfd (&fds)[1024] = *getMyFds();

	if (!client->getAuthenticated()) {
		sendBuffer += msg_err_notregistered();
		fds[i].events |= POLLOUT;
		std::cout << RED "Client not authenticated, sending error message." RESET << std::endl;
		return (false);
	}
	if (!client->getRegistered() || client->getNickName() == "*")
	{
		sendBuffer += msg_err_notregistered();
		sendBuffer += msg_notice("Please set your user: USER <username> <hostname> <servername> : <realname>");
		sendBuffer += msg_notice("Please set your nick: NICK <nickname>");
		std::cout << RED "Client not registered, sending error message." RESET << std::endl;
		fds[i].events |= POLLOUT;
		return (false);
	}
	return (true);
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

					std::string	name = (*clients)[fds[index].fd]->getUserName();
					std::cout << BRIGHT_GREEN << (name.empty() ? "Client": name) << ": " << YELLOW << fds[index].fd << LIGHT_BLUE << " " << line << RESET << std::endl;

					Client* client = (*clients)[fds[index].fd];
					if (handleCommands(clients, line, fds[index].fd, index)
						|| !checkSignUp(client, this->sendBuffer[index], index))
						continue;

					this->sendBuffer[index].clear();
					if (!isEmptyInput(line))
					this->sendBuffer[index] += "\n" + std::string(YELLOW) + (*clients)[fds[index].fd]->getNickName() + RESET + ": " + line;
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
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->begin();
	int	index;
	ssize_t	bytes;
	ssize_t	bytes2;
	struct pollfd (&fds)[1024] = *getMyFds();

	index = 0;
	while (*this->running && index < this->numClients && fds[index].fd != -1)
	{
		if (fds[index].revents & POLLOUT)
		{
			it = clients->find(fds[index].fd);
			/*
				This is a test-doesn't workd properly
				what it does
					shows
					Me: <your input>
				uncomment if u want to see
			*/
			// if (this->sendBuffer[index].find(":") == std::string::npos)
			// 	this->sendBuffer[index] = std::string(GREEN) + "Me: " + RESET + this->sendBuffer[index];
			bytes = send(fds[index].fd, this->sendBuffer[index].c_str(), sendBuffer[index].size(), 0);
			if (bytes > 0)
				this->sendBuffer[index].erase(0, bytes);
			bytes2 = send(fds[index].fd, it->second->getBufferOut().c_str(), it->second->getBufferOut().size(), 0);
			if (bytes2 > 0)
				it->second->getBufferOut().erase(0, bytes2);
			if (this->sendBuffer[index].empty() && it->second->getBufferOut().empty())
				fds[index].events &= ~POLLOUT;
		}
		++index;
	}
}
