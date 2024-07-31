# ft_irc

```
FINAL GRADE: 115/100
```

ft_irc is a group project which, as its name suggests, makes us recreate an IRC server (from scratch, in C++). The server follows the [RFC 2812](https://www.rfc-editor.org/rfc/rfc2812) specification and can therefore be used with existing IRC clients.

The project follows the subject, and also exceeds all the proposed bonuses (even if it does not yet contain all the features available in a real IRC server).

## Usage

### Build and start the server

```bash
make
./ircserv <port> <password>
# ./ircserv 8080 pass123
```

```bash

# Join a channel
/join <#channelName>

# Send a private message to a client
/privmsg <userName> <message>

# Send a private message in a channel
/privmsg <#channelName> <message>

# Leave a channel
/part <#channelName>

# Kick a user from a channel 
/kick <userName> :reason or not

# Display or set a channel's topic 
/topic <#channelName> :newTopic

# Modify channel modes (i, t, k, o, l) 
/mode (+/-)i : Set/remove Invite-only channe
/mode (+/-)t : Set/remove the restrictions of the TOPIC command to channel operators
/mode (+/-)k : Set/remove the channel key (password)
/mode (+/-)o : Give/take channel operator privilege
/mode (+/-)l : Set/remove the user limit to channel

# Quit the IRC server 
/quit

# Invite a client to a channel
/invite <userName> <#channelName>

# Who is connected on a channel
/who <#channelName>

# Change the nickname of a client
/nick <New_Nickname>