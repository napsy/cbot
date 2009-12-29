/*
Copyright (c) 2009, Luka Napotnik <luka.napotnik@gmx.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "user.h"

int cbot_user_send(struct _network *network, const char *channel, 
        const char *user, const char *message)
{
}

int cbot_user_notice(struct _network *network, const char *user,
        const char *message)
{
    char irc_notice[256];
    int n;

    sprintf(irc_notice, "NOTICE %s :%s\r\n", user, message);

    n = write(network->conn.sd, irc_notice, strlen(irc_notice));
    if (n == 0)
        printf("Error sending PONG\n");
}

int cbot_user_kick(struct _network *network, const char *channel, 
        const char *user)
{
    char irc_kick[256];
    int n;

    sprintf(irc_kick, "KICK %s %s\r\n", channel, user);

    n = write(network->conn.sd, irc_kick, strlen(irc_kick));
    if (n == 0)
        printf("Error sending PONG\n");
}

