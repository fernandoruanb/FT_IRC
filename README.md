<p align="center">
  <img src="https://assets.content.technologyadvice.com/636456225511427113_2ddb67efee.webp" alt="Illustrative IRC example GIF" width="100%">
</p>
<p align="center">
<sub>IRC Server, the "ancient Discord".</sub>
</p>

<h1 align="center">
  <a href="https://github.com/fernandoruanb/ft_irc">
    <img src="https://github.com/ayogun/42-project-badges/raw/main/badges/ft_irce.png" alt="ft_irc badge" width="200">
  </a>
  <br>
  ft_irc
  <br>
</h1>

<h4 align="center">
  A minimal IRC server implementation in <a href="https://isocpp.org/" target="_blank">C++</a> compatible with standard IRC clients such as <code>Irssi</code>.
</h4>

<p align="center">
  <img src="https://img.shields.io/badge/Final%20Score-100%2F125-00C853?style=for-the-badge" alt="Final Score 100/125">
  <img src="https://img.shields.io/badge/Mandatory%20Project-IRC%20Server-6A5ACD?style=for-the-badge" alt="Mandatory Project IRC Server">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=cplusplus" alt="Language C++">
  <img src="https://img.shields.io/badge/Status-Completed-brightgreen?style=for-the-badge" alt="Status Completed">
</p>

# About the Project

The server implements the core parts of the IRC protocol and supports multiple clients, channels, and operator privileges using a non-blocking polling architecture.

## Features
- Non-blocking TCP server
- Poll-based event loop
- Multi-client support
- Channel management
- Channel and User modes

## Architecture
The server is centered by `Server` class.

### Class architecture
```mermaid
classDiagram

class Server {
    -map<int, Client*> clients
    -map<int, Channel*> channels
    -pollfd fds[1024]
    -string recvBuffer[1024]
    -string sendBuffer[1024]
    -string password
    -int port
    -int serverIRC
    -int numClients
    -int numChannels

    +startIRCService()
    +addNewClient(fd)
    +broadcast()
    +tryRegister()
    +handleCommands()
}

class Client {
    -int clientFD
    -string nickname
    -string username
    -string realname
    -bool authenticated
    -bool registered
    -bool isOperator
    -string bufferIn
    -string bufferOut
    -set<string> channels
    -set<string> operatorChannels

    +setNickName()
    +setUserName()
    +getNickName()
    +getBufferIn()
    +getBufferOut()
}

class Channel {
    -string name
    -string topic
    -string password
    -int userLimit
    -set<int> members
    -set<int> operators
    -set<int> invited
    -bool inviteFlag
    -bool topicFlag

    +addNewMember()
    +removeMember()
    +isMemberOfChannel()
    +setOperator()
}

Server --> Client
Server --> Channel
Channel --> Client
```

### Poll Event Loop

```mermaid
flowchart TD

Start[Server Start]

Init[Init Socket<br>Bind<br>Listen]

PollLoop["poll()"]

NewClient[Accept New Client]

ReadData[Read Client Buffer]

Parse[Parse Command]

Dispatch[handleCommands]

SendData[Write sendBuffer]

Shutdown[Signal Handler]

Start --> Init
Init --> PollLoop

PollLoop --> NewClient
PollLoop --> ReadData
PollLoop --> SendData

ReadData --> Parse
Parse --> Dispatch

Dispatch --> PollLoop
SendData --> PollLoop

PollLoop --> Shutdown
```


### Command Dispatch System

### Map
```mermaid
flowchart TD

Input[Command String]

Lookup[Map Lookup<br>map<string, member function>]

USER["user()"]
PING["handlePing()"]
MODE["mode()"]
NICK["nick()"]
JOIN["join()"]
PRIVMSG["privmsg()"]
KICK["kick()"]
INVITE["invite()"]
LIST["list()"]
WHO["who()"]
WHOIS["whois()"]
QUIT["quit()"]

Input --> Lookup

Lookup --> USER
Lookup --> PING
Lookup --> MODE
Lookup --> NICK
Lookup --> JOIN
Lookup --> PRIVMSG
Lookup --> KICK
Lookup --> INVITE
Lookup --> LIST
Lookup --> WHO
Lookup --> WHOIS
Lookup --> QUIT
```

### Command flow
```mermaid
flowchart TD

A[Client Message Buffer]

B[Extract Command]
C{Command in Map?}

D[Return False]

E{Client Authenticated?}

F{Command allowed?<br>CAP PASS QUIT}

G[ERR_NOTREGISTERED]

H{Client Registered?}

I{Command allowed?<br>CAP USER NICK QUIT}

J[ERR_NOTREGISTERED]

K[Set isOnline = true]

L[Execute Command<br>Server::*handler]

M{Client Online?}

N{Joined generic?}

O[Auto JOIN generic]

P{Just Registered?}

Q[Send Welcome + MOTD]

R[Set POLLOUT]
S[Return true]

A --> B
B --> C

C -- No --> D
C -- Yes --> E

E -- No --> F
F -- No --> G
F -- Yes --> L

E -- Yes --> H
H -- No --> I
I -- No --> J
I -- Yes --> L

H -- Yes --> K
K --> L

L --> M
M -- No --> R

M -- Yes --> N
N -- No --> O
N -- Yes --> P

O --> P

P -- Yes --> Q
P -- No --> R

Q --> R
R --> S
```

### Client Registration Flow
```mermaid
sequenceDiagram

participant Client
participant Server

Client->>Server: PASS password
Client->>Server: NICK nickname
Client->>Server: USER username

Server->>Server: tryRegister()

Server->>Client: 001 Welcome
Server->>Client: 002 YourHost
Server->>Client: 003 Created
Server->>Client: 004 ServerInfo
```


### Responsabilities
- Manage socket connections
- Parse IRC commands
- Maintain client state
- Maintain channel state
- Broadcast messages
- Handle auth and registration

## How to connect

### Run server
- Run `make` to compile
- `./ircserv <port> <pass>`
- Example `./ircserv 6667 12345`

### Connect
- Open `irssi`
- Run `/connect 127.0.0.1 <port> <password>`

