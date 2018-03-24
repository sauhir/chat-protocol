
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
    //
    strcat(message_str, ":");
    strcat(message_str, message->token);
    strcat(message_str, ":");
    strcat(message_str, message->nickname);
    strcat(message_str, ":");
    strcat(message_str, message->message);
    return message_str;
}
