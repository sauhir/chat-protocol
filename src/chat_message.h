/* Chat message structure and related functionality.
 *
 * Copyright (C) 2018 Sauli Hirvi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

/* Chat message */
typedef struct _chatMessage {
    char *token;
    char *nickname; /* Sender's nickname */
    char *message;  /* Chat message contents */
} chatMessage;

/*
Structure of a message body is as follows:
:token:nickname:this is the message text
*/
typedef struct _Message {
    int token_offset;
    int token_length;
    int nick_offset;
    int nick_length;
    int message_offset;
    int message_length;
} Message;

char *format_message(chatMessage *message);
chatMessage *parse_message(char *message);

#endif
