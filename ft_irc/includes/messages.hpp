#pragma once
#include <string>

struct	s_commands;

std::string msg_welcome(const std::string& nick);
std::string msg_yourhost(const std::string& nick);
std::string msg_created();
std::string msg_svrinfo();
std::string msg_err_needmoreparams(const std::string& cmd);
std::string msg_err_passwdmismatch();
std::string msg_err_nicknameinuse(const std::string& nick);
std::string msg_err_notregistered();
std::string msg_err_unknowncommand(const std::string& cmd);
std::string msg_err_nosuchchannel(const std::string& channel);
std::string msg_err_notonchannel(const std::string& channel);
std::string msg_err_channelisfull(const std::string& channel);
std::string msg_err_chanoprivsneeded(const std::string& channel);
std::string msg_err_erroneusnickname(const std::string& nick);
std::string msg_notice(const std::string& text);
std::string	msg_error(const std::string &message, int error_code, s_commands& com);
void	callCmdMsg(const std::string &message, int error, s_commands& com, std::string &buffer);
