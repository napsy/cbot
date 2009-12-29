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

void show_about(struct _network *network, struct _cbot_irc_message *msg)
{
    cbot_channel_send(network, msg->source, "cbot 0.0.1 at your service!");
}

char *check_db(const char *topic)
{
    FILE *db_file = fopen("tell_db.txt", "r");
    char *line = malloc(512),
         db_topic[128],
         *db_answer[512],
         *tmp;

    if (!db_file) {
        printf("* error opening \'tell_db.txt\' for \'!tell\' command\n");
        return NULL;
    }

    while (fgets(line, 512, db_file)) {
        if (strcasestr(line, topic) == line) {
            tmp = strchr(line, '=');
            if (!tmp) {
                printf("* no \'=\' character found in !tell DB entry\n");
                fclose(db_file);
                free(line);
                return NULL;
            }
            fclose(db_file);
            free(line);
            return strdup(tmp + 1);
        }
    }

    free(line);
    fclose(db_file);
    return NULL;
}

void help_user(struct _network *network, struct _cbot_irc_message *msg)
{
    char *tmp,
         *token,
         *target_nick = NULL,
         *answer = NULL,
         *topic = NULL,
         *message = msg->message;

    token = strtok(message, " \r", &tmp);

    while ((token = strtok(NULL, " \r", &tmp))) {
        if (!target_nick)
            target_nick = strdup(token);
        else if (!topic)
            topic = strdup(token);
    }

    answer = check_db(topic);

    if (answer) {
        int answer_len = strlen(target_nick) + strlen(answer) + 4;
        char *answer_msg = malloc(answer_len);
        sprintf(answer_msg, "%s: %s", target_nick, answer);
        cbot_channel_send(network, msg->source, answer_msg);
        free(answer_msg);
        free(answer);
    }
    
    if (target_nick)
        free(target_nick);
    if (topic)
        free(topic);
}
int main(int argc, char **argv)
{
    GMainLoop *loop;

    
    g_thread_init(NULL);
    loop = g_main_loop_new(NULL, TRUE);

    cbot_bot_commands = NULL;
    struct _client *client = cbot_client_new("localhost", "tartarus-bot", "tartarus-bot",
            "cbot");
    struct _network *network = cbot_network_new("irc.freenode.net", 6667,
            client, my_cb);

    cbot_bot_register_command("!about", 0, NULL, show_about);
    cbot_bot_register_command("!tell", 0, NULL, help_user);
    cbot_irc_register(network);
    cbot_channel_join(network, "#cbot-test");

    g_main_loop_run(loop);
    return 0;
}

