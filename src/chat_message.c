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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chat.h"
#include "chat_message.h"

char *format_message(chatMessage *message) {
    size_t msg_size = strlen(message->token) + strlen(message->nickname) +
                      strlen(message->message_type) + strlen(message->message) +
                      5; /* 5 = 4 colons and newline */

    char *message_str = calloc(msg_size, sizeof(char));
    /* Concatenate the message components into a colon-delimited string */
    strcat(message_str, ":");
    strcat(message_str, message->token);
    strcat(message_str, ":");
    strcat(message_str, message->nickname);
    strcat(message_str, ":");
    strcat(message_str, message->message_type);
    strcat(message_str, ":");
    strcat(message_str, message->message);
    strcat(message_str, "\n");
    return message_str;
}

/*
 * Parse message structure.
 * Splits message into tokens delimited by colon (:)
 * Returns tokenized pointer array.
 */
chatMessage *parse_message(char *message) {
    char *ptr_start;
    char *ptr_end;
    int len;

    if (strlen(message) == 0) {
        /* Empty string */
        return NULL;
    }

    chatMessage *msg = malloc(sizeof(chatMessage));

    /*
     * Parse access token
     */
    ptr_start = message + 1;            /* Skip over the colon */
    ptr_end = strchr(message + 1, ':'); /* Find next colon */

    len = ptr_end - ptr_start;

    char *token = calloc(1, len + 1); /* Allocate memory for the string */
    memcpy(token, ptr_start, len);    /* Copy the string */
    msg->token = token;

    /*
     * Parse nickname
     */
    ptr_start = ptr_end + 1;
    ptr_end = strchr(ptr_start + 1, ':');

    len = ptr_end - ptr_start;

    char *nickname = calloc(1, len + 1);
    memcpy(nickname, ptr_start, len);
    msg->nickname = nickname;

    /*
     * Parse message type
     */
    ptr_start = ptr_end + 1;
    ptr_end = strchr(ptr_start + 1, ':');

    len = ptr_end - ptr_start;

    char *message_type = calloc(1, len + 1);
    memcpy(message_type, ptr_start, len);
    msg->message_type = message_type;

    /*
     * Parse message
     */
    ptr_start = ptr_end + 1;
    ptr_end = strchr(ptr_start + 1, '\n'); /* Find ETX character */
    len = ptr_end - ptr_start;

    char *message_str = calloc(1, len + 1);
    memcpy(message_str, ptr_start, len);

    msg->message = message_str;
    return msg;
}
