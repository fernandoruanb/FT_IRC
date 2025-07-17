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
					// this->sendBuffer[index] = name + ": " + this->sendBuffer[index];

				   if (!handleClientAuthentication(clients, fds[index].fd, (char*)line.c_str(), index)) {
					   continue;
				   }

				//    user(clients, line, fds[index].fd, index);
				 if (handleCommands(clients, line, fds[index].fd, index)) {
					   continue;
				   }

				   Client* client = (*clients)[fds[index].fd];
				   if (!client->getAuthenticated() || !client->getRegistered())
					   continue;
				//    if (line.rfind("NICK ", 0) == 0) {
				// 	   handleNick(clients, fds[index].fd, line, index);
				// 	   continue;
				//    }
					// std::string cmd = line;
					// cmd.erase(cmd.find_last_not_of("\r\n") + 1); // Remove \r\n do final

					// if (cmd.compare(0, 4, "PING") == 0) {
					// 	s_commands	com(line, clients, fds[index].fd, index);
					// 	// Agora aceita "PING", "PING ", "PING:..." etc.
					// 	handlePing(com);
					// 	continue;
					// }
				   this->sendBuffer[index].clear();
				   if (!isEmptyInput(line))
				   	this->sendBuffer[index] += "\n" + std::string(YELLOW) + (*clients)[fds[index].fd]->getUserName() + RESET + ": " + line;
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
			if (this->sendBuffer[index].empty())
				fds[index].events &= ~POLLOUT;
		}
		++index;
	}
}
