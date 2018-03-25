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
    size_t msg_size = sizeof(message->token) + sizeof(message->nickname) +
                      sizeof(message->message) + 3;

    char *message_str = calloc(msg_size, sizeof(char));
    /* Concatenate the message components into a colon-delimited string */
    strcat(message_str, ":");
    strcat(message_str, message->token);
    strcat(message_str, ":");
    strcat(message_str, message->nickname);
    strcat(message_str, ":");
    strcat(message_str, message->message);
    return message_str;
}

/*
 * Parse message structure.
 * Splits message into tokens delimited by colon (:)
 * Returns tokenized pointer array.
 */
chatMessage *parse_message(char *message) {
    char **tokens;
    char *token;
    char *tmp = message;
    int c;
    int length;
    size_t count = 0;

    length = sizeof(message);

    if (length == 0) {
        /* Empty string */
        return NULL;
    }

    while (*tmp) {
        if (strcmp(tmp, ":") == 0) {
            count++;
        }
        tmp++;
    }

    tokens = calloc(count, sizeof(char *));

    c = 0;
    /* get the first token */
    token = strtok(message, ":");

    chatMessage *msg = malloc(sizeof(chatMessage));

    /* Initialize message with empty values in case we cannot get all tokens */
    msg->token = "";
    msg->nickname = "";
    msg->message = "";

    while (token != NULL) {
        switch (c++) {
        case 0:
            msg->token = strdup(token);
            token = strtok(NULL, ":");
            break;
        case 1:
            msg->nickname = strdup(token);
            /* Use an ACK ascii code as tokenizer so that we never find it.
             * This causes the next token to be the whole remaining string */
            token = strtok(NULL, "\6");
            break;
        case 2:
            msg->message = strdup(token);
            break;
        }
        if (c > 2) {
            break;
        }
    }

    return msg;
}
