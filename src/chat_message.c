
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
    // Concatenate the message components into a colon-delimited string
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
        // printf("%d: %s\n", c, token);
        switch (c) {
        case 0:
            msg->token = strdup(token);
            break;
        case 1:
            msg->nickname = strdup(token);
            break;
        case 2:
            msg->message = strdup(token);
            break;
        }
        c++;
        token = strtok(NULL, ":");
    }

    return msg;
}
