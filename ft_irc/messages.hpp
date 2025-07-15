#pragma once
#include <string>

std::string msg_welcome(const std::string& nick);
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