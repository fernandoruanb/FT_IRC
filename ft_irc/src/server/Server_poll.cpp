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

static void	cleanEnd(std::string& line)
{
	while (!line.empty())
	{
		char	lastChar = line[line.size() - 1];

		if (lastChar == '\r' || lastChar == '\n' || lastChar == ' ')
		 	line.resize(line.size() - 1);
		else
			break;
	}
}

static std::string	getFirstArg(std::string &line)
{
	line.erase(0, line.find_first_not_of(" \t\r\n"));
	if (line.empty())
		return ("");

	size_t	pos = line.find(' ');
	std::string	result = line.substr(0, pos);

	for (size_t i = 0; i < result.size(); i++)
		if (!std::isprint(result[i]))
			return ("");

	return (result);
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
				this->recvBuffer[index] += buffer;

				size_t pos;
				while ((pos = this->recvBuffer[index].find('\n')) != std::string::npos)
				{
					std::string line = this->recvBuffer[index].substr(0, pos + 1);
					this->recvBuffer[index].erase(0, pos + 1);

					std::string	name = (*clients)[fds[index].fd]->getUserName();
					std::cout << BRIGHT_GREEN << (name.empty() ? "Client": name) << ": " << YELLOW << fds[index].fd << LIGHT_BLUE << " " << line << RESET << std::endl;					
					Client* client = (*clients)[fds[index].fd];
					if (handleCommands(clients, line, fds[index].fd, index)
						|| client->getRegistered() == false)
						continue;

					cleanEnd(line);
					std::string	firstArg = getFirstArg(line);
					if (!firstArg.empty())
						this->sendBuffer[index] += msg_err_unknowncommand(firstArg);
					fds[index].events |= POLLOUT;
				}
			}
			if (bytes == 0)
			{
				std::cout << LIGHT_BLUE "Client " << YELLOW << fds[index].fd << LIGHT_BLUE " disconnected" << RESET << std::endl;
				std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
				removeAllChannelsOfClient(it->first);
				(*clients)[it->first]->setAuthenticated(false);
				(*clients)[it->first]->setRegistered(false);
				(*clients)[it->first]->setNickName("*");
				(*clients)[it->first]->setUserName("*");
				(*clients)[it->first]->setHost("localhost");
				this->kingsOfIRC.erase(it->first);
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
	ssize_t	bytes3;
	struct pollfd (&fds)[1024] = *getMyFds();

	index = 0;
	while (*this->running && index < this->numClients && fds[index].fd != -1)
	{
		if (fds[index].revents & POLLOUT)
		{
			it = clients->find(fds[index].fd);
			bytes = send(fds[index].fd, this->sendBuffer[index].c_str(), sendBuffer[index].size(), 0);
			if (bytes > 0)
				this->sendBuffer[index].erase(0, bytes);
			bytes2 = send(fds[index].fd, it->second->getBufferOut().c_str(), it->second->getBufferOut().size(), 0);
			if (bytes2 > 0)
				it->second->getBufferOut().erase(0, bytes2);
			bytes3	= send(fds[index].fd, it->second->getSendHistory()[it->second->getChannelOfTime()].c_str(), it->second->getSendHistory()[it->second->getChannelOfTime()].size(), 0);
			if (bytes3 > 0)
				it->second->getSendHistory()[it->second->getChannelOfTime()].erase(0, bytes3);
			if (this->sendBuffer[index].empty() && it->second->getBufferOut().empty() && it->second->getSendHistory()[it->second->getChannelOfTime()].empty())
				fds[index].events &= ~POLLOUT;
		}
		++index;
	}
}
