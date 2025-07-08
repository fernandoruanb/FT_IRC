#include "Client.hpp"
#include "internet.hpp"

Client::Client(void)
{
	this->fds.fd = -1;
	this->_isOnline = false;
}
Client::Client(const Client &other)
{
	*this = other;
}
Client&	Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->fds.fd = other.getPollfd().fd;
		this->fds.events = other.getPollfd().events;
		this->fds.revents = other.getPollfd().revents;
		this->name = other.getName();
		this->_isOnline = other.isOnline();
	}
	return (*this);
}
Client::~Client(void){}


const bool&			Client::isOnline(void) const { return (this->_isOnline); }
const struct pollfd&	Client::getPollfd(void) const { return (this->fds); }
const std::string&	Client::getName(void) const { return (this->name); }
const std::string&	Client::getSendBuffer(void) const { return(this->sendBuffer); }
const std::string&	Client::getRecvBuffer(void) const { return (this->recvBuffer); }


bool	Client::hasDataToSend(void) { return (!this->sendBuffer.empty()); }

void	Client::addNewClient(void)
{
	t_server	*server = getServer();
	std::string	ask = "Insert your name: ";

	this->fds.fd = server->clFD;
	this->fds.events = POLLIN;
	fcntl(server->clFD, F_SETFL, O_NONBLOCK);
	server->nclFD++;

	send(this->fds.fd, ask.c_str(), ask.size(), 0);

	this->name.clear();
	this->_isOnline = true;
}

void	Client::removeClient(void)
{
	close(this->fds.fd);
	this->fds.fd = -1;
	this->_isOnline = false;
	this->name.clear();
}

void	Client::setName(const std::string &name)
{
	this->name = name;
}