#include "../includes/Server.hpp"
#include <sstream>

s_mode::s_mode(char s, char f, bool ff, Channel* &t, std::string& c, size_t l, int i)
    : sign(s), flag(f), flagFound(ff), target(t), currentMode(c), len(l), channelIndex(i)
{}

static bool	isSigned(const char c)
{
    return (c == '+' || c == '-');
}

static	Client	*getTargetClient(s_commands &com, const std::string& name, std::string &sendBuffer)
{
    std::map<int, Client*>::iterator	it;

    for (it = com.clients->begin(); it != com.clients->end(); it++)
        if (it->second->getNickName() == name)
            return (it->second);
    
    // Usar código de erro correto para nick não encontrado
    sendBuffer += ":" SERVER_NAME " 401 " + com.client->getNickName() + " " + name + " :No such nick/channel\r\n";
    return (NULL);
}

static Channel*	getTargetChannel(s_commands &com, std::map<int, Channel*>* &channels, std::string &sendBuffer)
{
    std::map<int, Channel*>::iterator	it;
    //	Removing # from #<chanelName>
    std::string	name = com.args[0].substr(1);

    for (it = channels->begin(); it != channels->end(); it++)
        if (it->second->getName() == name)
            return (it->second);
    
    // Usar código de erro correto para canal não encontrado
    sendBuffer += ":" SERVER_NAME " 403 " + com.client->getNickName() + " " + com.args[0] + " :No such channel\r\n";
    return (NULL);
}

static void	showModes(s_commands& com, std::string &sendBuffer, std::map<int, Channel*>* &channels, bool isUser)
{
    if (com.args.size() != 1)
        return;

    if (isUser)
    {
        Client*	target = getTargetClient(com, com.args[0], sendBuffer);
        if (target)
        {
            // Formato correto para resposta de modo de usuário
            std::string userModes = target->getMode(com.client->getChannelOfTime());
            if (userModes.empty())
                userModes = "";
            sendBuffer += ":" SERVER_NAME " 221 " + target->getNickName() + " +" + userModes + "\r\n";
        }
        return;
    }

    Channel*	target = getTargetChannel(com, channels, sendBuffer);
    if (target)
    {
        // Formato correto para resposta de modo de canal
        std::string channelModes = target->getMode();
        if (channelModes.empty())
            channelModes = "";
        sendBuffer += ":" SERVER_NAME " 324 " + com.client->getNickName() + " " + com.args[0] + " +" + channelModes + "\r\n";
    }
}

/*
    Check if (char)mode is in myModes

    how to use

        findMode(getMode(), 'o');
*/
bool	findMode(const std::string& myModes, const char mode)
{
    if (myModes.empty())
        return (false);

    for (size_t i = 0; i < myModes.size(); i++)
        if (myModes[i] == mode)
            return (true);
    
    return (false);
}

static void	addUserMode(Client* &target, s_commands &com, std::string &sendBuffer, std::map<int, Channel*>* &channels)
{
    if (com.args.size() < 2 || !isSigned(com.args[1][0]))
        return;
    
    // Apenas o próprio usuário pode alterar seus modos (exceto operadores)
    if (target != com.client && !com.client->getIsOperator())
    {
        sendBuffer += ":" SERVER_NAME " 502 " + com.client->getNickName() + " :Can't change mode for other users\r\n";
        return;
    }
    
    (void)channels;
    std::string	currentMode = target->getMode(com.client->getChannelOfTime());
    std::string	args = com.args[1].substr(1);
    char		sign = com.args[1][0];
    std::string	modeChanges = "";

    //	Check the target modes
    for (size_t i = 0; i < args.size(); i++)
    {
        char	mode = args[i];
        bool	isActive = findMode(currentMode, mode);

        // Validar modos de usuário válidos
        if (mode != 'i' && mode != 'o')
        {
            sendBuffer += ":" SERVER_NAME " 472 " + com.client->getNickName() + " " + mode + " :is unknown mode char to me\r\n";
            continue;
        }

        // Apenas operadores IRC podem dar/tirar privilégios de operador
        if (mode == 'o' && !com.client->getIsOperator())
        {
            sendBuffer += ":" SERVER_NAME " 481 " + com.client->getNickName() + " :Permission Denied- You're not an IRC operator\r\n";
            continue;
        }

        if (sign == '+' && !isActive)
        {
            currentMode += mode;
            modeChanges += std::string(1, sign) + std::string(1, mode);
        }
        else if (sign == '-' && isActive)
        {
            size_t	pos = currentMode.find(mode);
            currentMode.erase(pos, 1);
            modeChanges += std::string(1, sign) + std::string(1, mode);
        }
    }

    if (!modeChanges.empty())
    {
        target->setMode(currentMode, com.client->getChannelOfTime());
        if (findMode(target->getMode(com.client->getChannelOfTime()), 'o'))
        {
            target->setIsOperator(true);
        }
        // Resposta no formato correto
        sendBuffer += ":" + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + 
                      " MODE " + target->getNickName() + " " + modeChanges + "\r\n";
    }
}

static void	caseT(s_commands& com, s_mode& mode)
{
    (void)com; // Suppress unused parameter warning
    if (mode.sign == '+' && !mode.flagFound)
    {
        mode.currentMode += mode.flag;
        mode.target->setTopicFlag(true);
        return;
    }
    if (mode.sign == '-' && mode.flagFound)
    {
        size_t	pos = mode.currentMode.find(mode.flag);
        mode.currentMode.erase(pos, 1);
        mode.target->setTopicFlag(false);
    }
}

static void	caseK(s_commands& com, s_mode& mode)
{
    bool	hasPermission = findMode(com.client->getMode(mode.channelIndex), 'o') || com.client->getIsOperator();

    if (mode.sign == '+')
    {
        if (mode.len < 3)
        {
            com.sendBuffer += ":" SERVER_NAME " 461 " + com.client->getNickName() + " MODE :Not enough parameters\r\n";
            return;
        }
        if (hasPermission)
        {
            if (!mode.flagFound)
                mode.currentMode += mode.flag;
            mode.target->setPassWord(com.args[2]);
            return;
        }
        com.sendBuffer += ":" SERVER_NAME " 482 " + com.client->getNickName() + " " + com.args[0] + " :You're not channel operator\r\n";
        return;
    }
    if (mode.sign == '-' && mode.flagFound)
    {
        if (hasPermission)
        {
            size_t	pos = mode.currentMode.find(mode.flag);
            mode.currentMode.erase(pos, 1);
            mode.target->getPassWord().clear();
            return;
        }
        com.sendBuffer += ":" SERVER_NAME " 482 " + com.client->getNickName() + " " + com.args[0] + " :You're not channel operator\r\n";
    }
}

static void	caseL(s_commands& com, s_mode& mode)
{
    if (mode.sign == '+' && !mode.flagFound)
    {
        if (mode.len < 3)
        {
            com.sendBuffer += ":" SERVER_NAME " 461 " + com.client->getNickName() + " MODE :Not enough parameters\r\n";
            return;
        }
        int	limit;
        std::istringstream	ss(com.args[2]);

        ss >> limit;
        if (ss.fail() || limit <= 0)
        {
            com.sendBuffer += ":" SERVER_NAME " 461 " + com.client->getNickName() + " MODE :Invalid limit value\r\n";
            return;
        }
        mode.currentMode += mode.flag;
        mode.target->setUserLimit(limit);
        return;
    }
    if (mode.sign == '-' && mode.flagFound)
    {
        size_t	pos = mode.currentMode.find(mode.flag);
        mode.currentMode.erase(pos, 1);
        mode.target->setUserLimit(1024);
    }
}

static void	caseO(s_commands& com, s_mode& mode)
{
    if (mode.len < 3)
    {
        com.sendBuffer += ":" SERVER_NAME " 461 " + com.client->getNickName() + " MODE :Not enough parameters\r\n";
        return;
    }
    
    Client*	client = getTargetClient(com, com.args[2], com.sendBuffer);
    if (!client)
        return;
    
    // Verificar se o cliente está no canal
    if (!mode.target->isMemberOfChannel(client->getClientFD()) && !mode.target->isOperatorOfChannel(client->getClientFD()))
    {
        com.sendBuffer += ":" SERVER_NAME " 441 " + com.client->getNickName() + " " + com.args[2] + " " + com.args[0] + " :They aren't on that channel\r\n";
        return;
    }
    
    if (mode.sign == '+')
    {
        client->setIsOperator(true);
        client->addMode(mode.flag, mode.channelIndex);
        mode.target->setOperator(client->getClientFD());
        if (!mode.flagFound)
            mode.currentMode += mode.flag;
    }
    else if (mode.sign == '-')
    {
        client->setIsOperator(false);
        mode.target->getOperatorsSet().erase(client->getClientFD());
        if (mode.flagFound)
        {
            size_t pos = mode.currentMode.find(mode.flag);
            mode.currentMode.erase(pos, 1);
        }
    }
}

static void	caseI(s_commands& com, s_mode& mode)
{
    (void)com; // Suppress unused parameter warning
    if (mode.sign == '+' && !mode.flagFound)
    {
        mode.currentMode += mode.flag;
        mode.target->setInviteFlag(true);
        return;
    }
    if (mode.sign == '-' && mode.flagFound)
    {
        size_t	pos = mode.currentMode.find(mode.flag);
        mode.currentMode.erase(pos, 1);
        mode.target->setInviteFlag(false);
    }
}

static void	addChannelMode(s_commands &com, Channel* &target, int channelIndex)
{
    // Verificar se o usuário é operador do canal
    std::string	userMode = com.client->getMode(channelIndex);
    bool isChannelOp = findMode(userMode, 'o') || target->isOperatorOfChannel(com.client->getClientFD());
    
    if (target->getName() == "generic" || (!isChannelOp && !com.client->getIsOperator()))
    {
        com.sendBuffer += ":" SERVER_NAME " 482 " + com.client->getNickName() + " " + com.args[0] + " :You're not channel operator\r\n";
        return;
    }
    
    size_t		len = com.args.size();
    if (len < 2)
        return;

    std::string	currentMode = target->getMode();
    char		sign = com.args[1][0];
    std::string	flags = com.args[1].substr(1);
    std::string	modeResponse = "";
    std::string	modeParams = "";
    
    std::map<char, void (*)(s_commands&, s_mode&)>	myMap;
    myMap['i'] = &caseI;
    myMap['o'] = &caseO;
    myMap['k'] = &caseK;
    myMap['l'] = &caseL;
    myMap['t'] = &caseT;

    for (size_t i = 0; i < flags.size(); i++)
    {
        char	flag = flags[i];
        bool	flagFound = findMode(currentMode, flag);

        if (myMap.find(flag) == myMap.end())
        {
            com.sendBuffer += ":" SERVER_NAME " 472 " + com.client->getNickName() + " " + flag + " :is unknown mode char to me for " + com.args[0] + "\r\n";
            continue;
        }
        
        std::string oldMode = currentMode;
        s_mode	modeStruct(sign, flag, flagFound, target, currentMode, len, channelIndex);	
        myMap[flag](com, modeStruct);
        
        // Se o modo foi alterado, adicionar à resposta
        if (oldMode != currentMode)
        {
            modeResponse += std::string(1, sign) + std::string(1, flag);
            
            // Adicionar parâmetros se necessário
            if ((flag == 'k' && sign == '+') || (flag == 'l' && sign == '+') || flag == 'o')
            {
                if (flag == 'o' && len > 2)
                    modeParams += " " + com.args[2];
                else if (flag == 'k' && len > 2)
                    modeParams += " " + com.args[2];
                else if (flag == 'l' && len > 2)
                    modeParams += " " + com.args[2];
            }
        }
    }

    target->setMode(currentMode);
    
    // Enviar resposta apenas se houve mudanças
    if (!modeResponse.empty())
    {
        com.sendBuffer += ":" + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + 
                          " MODE " + com.args[0] + " " + modeResponse + modeParams + "\r\n";
    }
}

/*
    Handle user modes according to IRC RFC standards
*/
void	Server::mode(s_commands &com)
{
    if (!com.args.size())
    {
        com.sendBuffer += ":" SERVER_NAME " 461 " + com.client->getNickName() + " MODE :Not enough parameters\r\n";
        return;
    }

    bool	isUser = com.args[0][0] != '#';

    // Se só tem 1 argumento, mostrar modos atuais
    if (com.args.size() == 1)
    {
        showModes(com, com.sendBuffer, this->channels, isUser);
        return;
    }

    if (isUser)
    {
        Client*	target = getTargetClient(com, com.args[0], com.sendBuffer);
        if (!target)
            return;
        addUserMode(target, com, com.sendBuffer, this->channels);
        return;
    }
    
    Channel*	target = getTargetChannel(com, this->channels, com.sendBuffer);
    if (!target)
        return;
    addChannelMode(com, target, getChannelsIndex(target->getName()));
}