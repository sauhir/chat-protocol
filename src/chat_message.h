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

typedef enum {
    msg_type_normal = 0,
    msg_type_status = 1,
    msg_type_ping = 2,
    msg_type_unknown = 3
} msg_type;

/* Chat message */
typedef struct _chatMessage {
    char *token;
    char *nickname;        /* Sender's nickname */
    msg_type message_type; /* Message type enum */
    char *message;         /* Chat message contents */
} chatMessage;

char *format_message(chatMessage *message);
chatMessage *parse_message(char *message);

#endif
