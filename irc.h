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


#ifndef __IRC_H__
#define __IRC_H__

#include "network.h"
struct _cbot_irc_message {
    struct _network *network;
    //struct _channel *channel;
    char *source;
    char *nick;
    char *mask;
    char *message;
};

struct _cbot_bot_command {
    char *cmd;
    int arg_count;
    char *arg_format;
    void (*cb)();
};

GList *cbot_bot_commands;

int cbot_irc_register(struct _network *network);
int cbot_irc_pong(struct _network *network, const char *host);

struct _cbot_irc_message *cbot_irc_get_message(struct _network *network, char *msg);
int cbot_irc_check_message(struct _cbot_irc_message *chan_msg);
void cbot_irc_message_destroy(struct _cbot_irc_message *chan_msg);

void cbot_bot_register_command(const char *cmd, int arg_count,
        const char *arg_format, void *cmd_cb);
#endif

