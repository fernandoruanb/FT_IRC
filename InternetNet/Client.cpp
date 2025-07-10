/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:06:24 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/10 11:50:54 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

#pragma region utils and operators

void	Client::clear(void)
{
	this->clientFD = -1;
	this->index = -1;
	this->authenticated = false;
	this->isOperator = false;
	this->nickname.clear();
	this->username.clear();
	// this->realName.clear();
	this->sendBuffer.clear();
	this->recvBuffer.clear();
}

std::ostream& operator<<(std::ostream& out, const Client& other)
{
	out << other.getNickName();

	return (out);
}

#pragma endregion

#pragma region Constructors and destructor

Client::Client(void){
	this->clear();
}
Client::~Client(void){
	if (this->getClientFD() != -1)
		close(this->clientFD);
	this->clear();
}

Client::Client(const Client& other){
	*this = other;
}
Client& Client::operator=(const Client& other){
	if (this != &other)
	{
		this->clientFD = other.getClientFD();
		this->index = other.getIndex();
		this->authenticated = other.getAuthenticated();
		this->isOperator = other.getIsOperator();
		this->nickname = other.getNickName();
		this->username = other.getUserName();
		// this->realName = other.getRealName();
		this->sendBuffer = other.getSendBuffer();
		this->recvBuffer = other.getRecvBuffer();
	}

	return (*this);
}

#pragma endregion

#pragma region Setters

void	Client::setNickName(const std::string& name){
	this->nickname = name;
}
void	Client::setUserName(const std::string& name){
	this->username = name;
}
// void	Client::setRealName(const std::string& name){
// '	this->realName = name;
// }'
void	Client::setAuthenticated(bool flag){
	this->authenticated = flag;
}
void	Client::setClientFD(int fd){
	this->clientFD = fd;
}
void	Client::setRecvBuffer(const std::string& content){
	this->recvBuffer = content;
}
void	Client::setSendBuffer(const std::string& content){
	this->sendBuffer = content;
}

#pragma endregion

#pragma region Getters
int		Client::getClientFD(void) const { return (this->clientFD); }
bool	Client::getAuthenticated(void) const { return (this->authenticated); }
bool	Client::getIsOperator(void) const { return (this->isOperator); }
int		Client::getIndex(void) const { return (this->index); }

const std::string&	Client::getNickName(void) const { return (this->nickname); }
const std::string&	Client::getUserName(void) const { return (this->username); }
// const std::string&	Client::getRealName(void) const { return (this->realname); }
const std::string&	Client::getSendBuffer(void) const { return (this->sendBuffer); }
const std::string&	Client::getRecvBuffer(void) const { return (this->recvBuffer); }

#pragma endregion


#pragma region Mathods

void	Client::addNewClient(void){}
void	Client::removeClient(void){}
bool	Client::hasDataToSend(void){
	return (!this->getSendBuffer().empty());
}

#pragma endregion
