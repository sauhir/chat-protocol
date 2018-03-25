/* Chat client
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
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "chat.h"
#include "chat_message.h"
#include "client.h"

int printtime() {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("[%02d:%02d:%02d] ", timeinfo->tm_hour, timeinfo->tm_min,
           timeinfo->tm_sec);

    return 0;
}

chatSession *create_session() {
    chatSession *session = calloc(1, sizeof(chatSession));
    session->token = "";
    session->nickname = "";
    return session;
}

int handshake(int socket, chatSession *session) {
    char response[MAX_MSG];
    char *part;

    /* send greeting */
    send(socket, "AHOY", MAX_MSG, 0);
    /* receive response */
    printtime();
    printf("Greeting sent\n");

    recv(socket, response, MAX_MSG, 0);

    /* get first part of response */
    part = strtok(response, ":");

    if (strcmp(part, "AHOY-HOY") == 0) {
        printtime();
        printf("Correct response received\n");
        /* Response correct, get token */
        part = strtok(NULL, ":");
        char *token = calloc(1, sizeof(part));
        strcpy(token, part);
        session->token = token;
        printf("Authentication token: %s\n", session->token);
    } else {
        return -1;
    }
    return 0;
}

/*
 * Prompt for nickname and store it into the session.
 */
int set_nickname(chatSession *session) {
    char *nick;
    nick = calloc(MAX_NICK, sizeof(char));
    printf("Select a nickname:\n");
    scanf(" %99[^\n]", nick);
    session->nickname = nick;
    return 0;
}

int main() {
    struct sockaddr_in server_address;
    int network_socket;
    int status;
    ssize_t len;

    chatSession *session;

    char *server_response;
    char *input;

    /* Create a socket */
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Specify an address for the socket */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8002);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Connect to the server */
    status = connect(network_socket, (struct sockaddr *)&server_address,
                     sizeof(server_address));

    /* Check for connection error */
    if (status == -1) {
        printf("There was an error connecting to the remote socket.\n\n");
        exit(1);
    }

    /* Initialize the session variable */
    session = create_session();

    printtime();
    printf("Connected to the server\n");

    server_response = calloc(MAX_MSG, sizeof(char));
    input = calloc(MAX_MSG, sizeof(char));

    /* Shake hands with the server */
    status = handshake(network_socket, session);
    if (status == -1) {
        printtime();
        printf("Handshake failed.\n");
        exit(1);
    }

    /* Select nick for user */
    set_nickname(session);

    /* Main loop */
    while (1) {
        printf("Say something:\n");

        /* Read from stdin until newline.
           Regular plain scanf call would include the newline character
           in the input string. */
        scanf(" %99[^\n]", input);

        chatMessage *message = malloc(sizeof(chatMessage));
        message->token = session->token;
        message->nickname = session->nickname;
        message->message = input;

        char *msg_str = format_message(message);

        /* Send message to server */
        len = send(network_socket, msg_str, MAX_MSG, 0);

        /* Get response from server */
        len = recv(network_socket, server_response, MAX_MSG, 0);

        chatMessage *msg = parse_message(server_response);

        /* Print out the response */
        printtime();
        printf("<%s> %s\n", msg->nickname, msg->message);

        server_response = malloc(MAX_MSG);
        free(input);
        input = calloc(MAX_MSG, sizeof(char));
    }

    free(input);
    /* Close the socket */
    close(network_socket);

    return 0;
}
