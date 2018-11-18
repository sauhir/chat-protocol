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
#include "error_log.h"

char *msg_parse_token(char *param_ptr, char separator);

char *msg_type_string(msg_type message_type) {
    if (message_type == msg_type_normal) {
        return "normal";
    } else if (message_type == msg_type_status) {
        return "status";
    } else if (message_type == msg_type_ping) {
        return "ping";
    } else {
        return "unknown";
    }
}

/*
 * Returns a formatted string according to protocol spec. E.g.
 * :abcd1234:nickname:normal:This is a message
 */
char *format_message(chatMessage *message) {
    size_t msg_size = strlen(message->token) + strlen(message->nickname) +
                      strlen(msg_type_string(message->message_type)) +
                      strlen(message->message) +
                      5; /* 5 = 4 colons and newline */

    char *message_str = calloc(msg_size, sizeof(char));
    /* Concatenate the message components into a colon-delimited string */
    strcat(message_str, ":");
    strcat(message_str, message->token);
    strcat(message_str, ":");
    strcat(message_str, message->nickname);
    strcat(message_str, ":");
    strcat(message_str, msg_type_string(message->message_type));
    strcat(message_str, ":");
    strcat(message_str, message->message);
    strcat(message_str, "\n");
    return message_str;
}

/*
 * Parse message structure.
 * Splits message into tokens delimited by colon (:)
 * Returns a chatMessage struct.
 * Returns NULL if an error occurred while parsing.
 */
chatMessage *parse_message(char *message) {
    chatMessage *msg;
    char *msg_type_str;

    if (strlen(message) == 0) {
        /* Empty string */
        return NULL;
    }

    msg = malloc(sizeof(chatMessage));

    /* Parse access token */
    if ((msg->token = msg_parse_token(message, ':')) == NULL) {
        return NULL;
    }
    /* Parse nickname */
    if ((msg->nickname = msg_parse_token(NULL, ':')) == NULL) {
        return NULL;
    }
    /* Parse message type */
    if ((msg_type_str = msg_parse_token(NULL, ':')) == NULL) {
        return NULL;
    } else {
        if (strcmp(msg_type_str, "normal") == 0) {
            msg->message_type = msg_type_normal;
        } else if (strcmp(msg_type_str, "status") == 0) {
            msg->message_type = msg_type_status;
        } else if (strcmp(msg_type_str, "ping") == 0) {
            msg->message_type = msg_type_ping;
        } else {
            msg->message_type = msg_type_unknown;
        }
    }

    /* Parse message */
    if ((msg->message = msg_parse_token(NULL, '\n')) == NULL) {
        return NULL;
    }

    return msg;
}

/*
 * Parse single token.
 * Works similarly to strtok()
 * You must provide the param_ptr on first call.
 * Subsequent calls will be relative to that pointer.
 */
char *msg_parse_token(char *param_ptr, char separator) {
    int len;
    char *ptr_start;
    char *output_str;
    static char *ptr_end = NULL;

    if (ptr_end == NULL && param_ptr == NULL) {
        return NULL;
    }

    if (param_ptr != NULL) {
        ptr_end = param_ptr;
    }

    ptr_start = ptr_end + 1;
    ptr_end = strchr(ptr_start, separator);

    if (ptr_end == NULL) {
        return NULL;
    }

    len = ptr_end - ptr_start;

    output_str = calloc(1, len + 1);
    memcpy(output_str, ptr_start, len);
    return (output_str);
}
