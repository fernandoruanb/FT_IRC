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
	if (!client->getRegistered() || client->getNickName() == "*")
	{
		sendBuffer += msg_err_notregistered();
		sendBuffer += msg_notice("Please assure the following commands are set:");
		sendBuffer += msg_notice("Password: PASS <password>");
		sendBuffer += msg_notice("Nick: NICK <nickname>");
		sendBuffer += msg_notice("User: USER <username> <hostname> <servername> : <realname>");
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

					// print all members of all channels for debug:
					// if commanded DEBUG
					if (line.find("DEBUG") != std::string::npos)
					{
						std::map<int, Channel*>* channels = getChannelsMap();
						std::map<int, Client*>* clients = getClientsMap();
						std::map<int, Channel*>::iterator itc = channels->begin();

						std::cout << "=== DEBUG: Canais e seus membros ===" << std::endl;
						while (itc != channels->end())
						{
							Channel* channel = itc->second;
							std::cout << "Canal: #" << channel->getName() << std::endl;
							// Operadores
							std::set<int>& operators = channel->getOperatorsSet();
							if (operators.empty())
								std::cout << "  (sem operadores)" << std::endl;
							else
							{
								std::cout << "  Operadores:" << std::endl;
								for (std::set<int>::iterator ito = operators.begin(); ito != operators.end(); ++ito)
								{
									std::map<int, Client*>::iterator itcl = clients->find(*ito);
									if (itcl != clients->end())
										std::cout << "    @ " << itcl->second->getNickName() << std::endl;
								}
							}
							// Membros
							std::set<int>& members = channel->getMembersSet();
							if (members.empty())
								std::cout << "  (sem membros)" << std::endl;
							else
							{
								std::set<int>::iterator itm = members.begin();
								while (itm != members.end())
								{
									std::map<int, Client*>::iterator itcl = clients->find(*itm);
									if (itcl != clients->end())
										std::cout << "  - " << itcl->second->getNickName() << std::endl;
									++itm;
								}
							}
							++itc;
						}
						std::cout << "=== Fim do DEBUG ===" << std::endl;
							continue; 
					}
					Client* client = (*clients)[fds[index].fd];
					if (handleCommands(clients, line, fds[index].fd, index)
						|| !checkSignUp(client, this->sendBuffer[index], index))
						continue;

					this->sendBuffer[index].clear();
					if (!isEmptyInput(line))
					this->sendBuffer[index] += std::string("\n:") + (*clients)[fds[index].fd]->getNickName() + "!" + (*clients)[fds[index].fd]->getUserName() + "@" + (*clients)[fds[index].fd]->getHost() + " PRIVMSG";
					this->broadcast(index, line);
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
	ssize_t	bytes3;
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
			bytes3	= send(fds[index].fd, it->second->getSendHistory()[it->second->getChannelOfTime()].c_str(), it->second->getSendHistory()[it->second->getChannelOfTime()].size(), 0);
			if (bytes3 > 0)
				it->second->getSendHistory()[it->second->getChannelOfTime()].erase(0, bytes3);
			if (this->sendBuffer[index].empty() && it->second->getBufferOut().empty() && it->second->getSendHistory()[it->second->getChannelOfTime()].empty())
				fds[index].events &= ~POLLOUT;
		}
		++index;
	}
}
