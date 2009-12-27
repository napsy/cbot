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

#include <time.h>

#include "channel.h"

int cbot_channel_join(struct _network *network, const char *channel)
{
    char irc_join[64];
    int n;
    struct _channel *chan;

    sprintf(irc_join, "JOIN %s\r\n", channel);
    
    n = write(network->conn.sd, irc_join, strlen(irc_join));
    if (n == 0)
        printf("Error sending NICK\n");

    chan = malloc(sizeof(*channel));
    chan->name = strdup(channel);
    chan->flags = 0;
    chan->message_count = 0;
    chan->last_message = NULL;
    network->channels = g_list_append(network->channels, chan);
}

int cbot_channel_part(struct _network *network, const char *channel)
{
}
int cbot_channel_send(struct _network *network, const char *channel,
        const char *message)
{
    char irc_send[512];
    int n;

    sprintf(irc_send, "PRIVMSG %s :%s\r\n", channel, message);

    n = write(network->conn.sd, irc_send, strlen(irc_send));
    if (n == 0)
        printf("Error sending PRIVMSG\n");

}

int cbot_channel_antiflood(struct _network *network, const char *channel,
        struct _cbot_irc_message *cur_msg)
{
    GList *list_item;
    double elapsed;
    time_t cur_time;
    struct _channel *chan;
    int the_same = 0;

    for (list_item = network->channels; list_item; list_item = list_item->next)
    {
        chan = (struct _channel *)list_item->data;
        if (strcmp(chan->name, channel) == 0)
            break;
        
    }

    time(&cur_time);
    if (!chan->last_message) {
        chan->last_message = cur_msg;
        chan->message_time = cur_time;
        return 0;
    }

    if ((strcmp(chan->last_message->source, cur_msg->source) == 0) &&
                (strcmp(chan->last_message->nick, cur_msg->nick) == 0)) {

        elapsed = difftime(cur_time, chan->message_time);
        printf("Elapsed: %f\n", elapsed);
        if (elapsed <= 2)
            chan->message_count++;
        else
            chan->message_count = 0;
    } 

    if (chan->message_count >= 4)  {
        cbot_channel_send(network, chan->name, "STOP FLOODING, IDIOT!");
        chan->message_count = 0;
    }

    cbot_irc_message_destroy(chan->last_message);
    chan->last_message = cur_msg;

    chan->message_time = cur_time;


    return 0;
}

