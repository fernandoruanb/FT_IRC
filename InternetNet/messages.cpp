#include <string>
#include "Server.hpp" // Certifique-se de que SERVER_NAME está definido aqui

// Mensagem de boas-vindas após registro completo
std::string msg_welcome(const std::string& nick) {
    return ":" SERVER_NAME " 001 " + nick + " :Welcome to the IRC Network, " + nick + "!\r\n";
}

// Mensagem de erro: senha obrigatória
std::string msg_err_needmoreparams(const std::string& cmd) {
    return ":" SERVER_NAME " 461 " + cmd + " :Not enough parameters\r\n";
}

// Mensagem de erro: senha incorreta
std::string msg_err_passwdmismatch() {
    return ":" SERVER_NAME " 464 * :Password incorrect\r\n";
}

// Mensagem de erro: nickname já em uso
std::string msg_err_nicknameinuse(const std::string& nick) {
    return ":" SERVER_NAME " 433 * " + nick + " :Nickname is already in use\r\n";
}

// Mensagem de erro: não registrado
std::string msg_err_notregistered() {
    return ":" SERVER_NAME " 451 * :You have not registered\r\n";
}

// Mensagem de erro: comando desconhecido
std::string msg_err_unknowncommand(const std::string& cmd) {
    return ":" SERVER_NAME " 421 * " + cmd + " :Unknown command\r\n";
}

// Mensagem de erro: canal inexistente
std::string msg_err_nosuchchannel(const std::string& channel) {
    return ":" SERVER_NAME " 403 * " + channel + " :No such channel\r\n";
}

// Mensagem de erro: não está em canal
std::string msg_err_notonchannel(const std::string& channel) {
    return ":" SERVER_NAME " 442 * " + channel + " :You're not on that channel\r\n";
}

// Mensagem de erro: canal cheio
std::string msg_err_channelisfull(const std::string& channel) {
    return ":" SERVER_NAME " 471 * " + channel + " :Cannot join channel (+l)\r\n";
}

// Mensagem de erro: sem permissão de operador
std::string msg_err_chanoprivsneeded(const std::string& channel) {
    return ":" SERVER_NAME " 482 * " + channel + " :You're not channel operator\r\n";
}

// Mensagem de erro: nickname inválido
std::string msg_err_erroneusnickname(const std::string& nick) {
    return ":" SERVER_NAME " 432 * " + nick + " :Erroneous nickname\r\n";
}

// Mensagem NOTICE genérica
std::string msg_notice(const std::string& text) {
    return ":" SERVER_NAME " NOTICE * :" + text + "\r\n";
}