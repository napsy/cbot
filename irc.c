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


#include "irc.h"

#ifdef __linux__
#include <unistd.h>
#endif

int cbot_irc_register(struct _network *network)
{
    int n;
    char irc_user[128];
    char *irc_nick[128];

    sprintf(irc_user, "USER %s %s %s :%s\r\n", network->client->user,
            network->client->hostname, network->hostname,
            network->client->realname);
   
    sprintf(irc_nick, "NICK %s\r\n", network->client->nick);

    n = write(network->conn.sd, irc_user, strlen(irc_user));
    if (n == 0)
        printf("Error sending USER\n");
    n = write(network->conn.sd, irc_nick, strlen(irc_nick));
    if (n == 0)
        printf("Error sending NICK\n");

}

int cbot_irc_pong(struct _network *network, const char *host)
{
    char irc_pong[64];
    int n;

    sprintf(irc_pong, "PONG %s\r\n", host);

    n = write(network->conn.sd, irc_pong, strlen(irc_pong));
    if (n == 0)
        printf("Error sending PONG\n");
}

struct _cbot_irc_message *cbot_irc_get_message(struct _network *network, char *msg)
{
    struct _cbot_irc_message *chan_msg = malloc(sizeof(*chan_msg));
    char token[512];
    int i = 0;
    char *tmp = msg;

    chan_msg->nick = malloc(16);
    chan_msg->mask = malloc(64);
    chan_msg->source = malloc(16);
    chan_msg->message = malloc(512);
    chan_msg->network = network;

    memset(chan_msg->nick, 0, 16);
    memset(chan_msg->mask, 0, 64);
    memset(chan_msg->source, 0, 16);

    while (*msg != '\0') {
        if (*msg == '!' && !chan_msg->nick[0]) {
            token[i] = 0;
            strncpy(chan_msg->nick, token+1, 15);
            i = 0;
        }
        else if (*msg == ' ' && !chan_msg->mask[0]) {
            token[i] = 0;
            strncpy(chan_msg->mask, token+2, 64);
            i = 0;
            msg = strcasestr(msg, "PRIVMSG");
            msg += 7; /* skip to channel*/
        }
        else if (*msg == ' ' && !chan_msg->source[0]) {
            token[i] = 0;
            strncpy(chan_msg->source, token, 15);
            i = 0;
        }
        else
            token[i++] = *msg;
        msg++;
    }
    token[i] = 0;
    strncpy(chan_msg->message, token+1, 511);

    char prefix = chan_msg->source[0];
    if (prefix == '#' || prefix == '&' || prefix == '!') {
        printf("%s - <%s> %s", chan_msg->source, chan_msg->nick,
                chan_msg->message);
    } else {
        printf("PRIVATE <%s> %s", chan_msg->nick,
                chan_msg->message);

    }

    cbot_irc_check_for_commands(network, chan_msg);
    return chan_msg;
}

void cbot_irc_check_for_commands(struct _network *network, struct
        _cbot_irc_message *msg)
{
    GList *tmp = cbot_bot_commands;
    gboolean found = FALSE;
    struct _cbot_bot_command *cmd;

    for (; tmp; tmp = tmp->next) {
        cmd = (struct _cbot_bot_command *)tmp->data;
        if (strcasestr(cmd->cmd, msg->message)) {
            found = TRUE;
            break;
        }

    }
    if (found)
        cmd->cb();
}
int cbot_irc_check_message(struct _cbot_irc_message *chan_msg)
{
    if (strchr(chan_msg->message, '!') == chan_msg->message) {
        if (strcasecmp(chan_msg->message + 1, "about\r\n") == 0) {
            printf("Sending response ...");
            cbot_channel_send(chan_msg->network, chan_msg->source,
                    "cbot 0.0.1 at your service");
        }
    }
}

void cbot_irc_message_destroy(struct _cbot_irc_message *chan_msg)
{
    free(chan_msg->nick);
    free(chan_msg->source);
    free(chan_msg->message);
    free(chan_msg->mask);
    free(chan_msg);
}

void cbot_bot_register_command(const char *cmd, int arg_count,
        const char *arg_format, void *cmd_cb)
{
    struct _cbot_bot_command *bot_cmd = malloc(sizeof(*bot_cmd));

    bot_cmd->cmd = strdup(cmd);
    bot_cmd->arg_format = strdup(arg_format);
    bot_cmd->arg_count = arg_count;
    bot_cmd->cb = cmd_cb;

    cbot_bot_commands = g_list_append(cbot_bot_commands, bot_cmd);

}

