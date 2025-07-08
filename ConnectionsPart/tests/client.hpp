#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <unistd.h>

class Client
{
	private:
		std::string	name;
		int			fd;
		bool		_isOnline;
	public:
		Client(void);
		Client(const Client &other);
		Client&	operator=(const Client &other);
		~Client(void);

		//getters
		const bool&			isOnline(void) const;
		const int&			getFd(void) const;
		const std::string&	getName(void) const;

		//methods
		void	addNewClient(const std::string &name, int &fd);
		void	removeClient(void);
};

Client::Client(void)
{
	this->fd = -1;
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
		this->fd = other.getFd();
		this->name = other.getName();
		this->_isOnline = other.isOnline();
	}
	return (*this);
}
Client::~Client(void){}


const bool&			Client::isOnline(void) const { return(this->_isOnline); }
const int&			Client::getFd(void) const { return(this->fd); }
const std::string&	Client::getName(void) const { return (this->name); }

void	Client::addNewClient(const std::string &name, int &fd)
{
	this->fd = fd;
	this->name = name;
	this->_isOnline = true;
}

void	Client::removeClient(void)
{
	close(this->fd);
	this->fd = -1;
	this->_isOnline = false;
	this->name.clear();
}

#endif
