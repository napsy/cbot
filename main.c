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


#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "network.h"
#include "user.h"
#include "channel.h"
#include "client.h"
#include "irc.h"

void *my_cb(struct _network *network, void *data)
{
    char *msg = (char *)data;
    char *tmp;
    struct _cbot_irc_message *chan_msg = NULL,
                             *cur_chan_msg;
    
    int msg_limit = 5, msg_count = 0;
    double elapsed, limit = 2;
    GTimer *timer = g_timer_new();

    if ((tmp = strcasestr(msg, "PING")) == msg) {
        tmp += 6;
        printf("PONG ...\n");
        cbot_irc_pong(network, tmp);
    }
    else if (strchr(msg, ':') == msg) {

        if ((tmp = strcasestr(msg, "PRIVMSG"))) {
            tmp += 8;
            cur_chan_msg = cbot_irc_get_message(network, msg);
            cbot_channel_antiflood(network, cur_chan_msg->source,
                    cur_chan_msg);
        }
    } else
        printf("%s", msg);
    free(msg);
}

int main(int argc, char **argv)
{
    GMainLoop *loop;
    cbot_bot_commands = NULL;
    
    g_thread_init(NULL);
    loop = g_main_loop_new(NULL, TRUE);

    struct _client *client = cbot_client_new("localhost", "tartarus-bot", "tartarus-bot",
            "cbot");
    struct _network *network = cbot_network_new("irc.freenode.net", 6667,
            client, my_cb);
    cbot_irc_register(network);
    cbot_channel_join(network, "#cbot-test");
    g_main_loop_run(loop);
    return 0;
}

