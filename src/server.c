/* Chat server
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

#include <netinet/in.h>

#include "chat.h"
#include "chat_message.h"
#include "server.h"
#include "server_commands.h"
#include "session.h"

/*
Protocol handshake:
Client sends
AHOY
Server responds
AHOY-HOY:[random access token]

The client must store the token and include it in subsequent requests.
*/

/*
 * Send handshake response.
 */
int handshake(int socket) {
    printf("handshake()\n");

    char *token;
    char *buf;
    size_t len;

    token = (char *)calloc(9, sizeof(char));
    len = snprintf(NULL, 0, "AHOY-HOY:%s", create_token(token, 8));
    buf = (char *)calloc(len + 1, sizeof(char));
    memset(buf, 0, len + 1);
    snprintf(buf, len + 1, "AHOY-HOY:%s", create_token(token, 8));
    printf("buf:%s\n", buf);

    send(socket, buf, strlen(buf), 0);
    return 0;
}

/*
 * Open client socket connection
 */
int open_client_socket(int server_socket) {
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        printf("Unable to accept coonection on socket\n");
        exit(1);
    }
    return client_socket;
}

/*
 * Bind server socket and start listening
 * Returns the socket
 */
int init_server_socket() {
    struct sockaddr_in server_address; /* server socket parameters */
    int server_socket;                 /* socket for sending */
    int status;                        /* status code for function returns */

    /* Create the socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        printf("Error creating socket.\n");
        exit(1);
    }

    /* Specify address settings for the socket */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /* Bind the socket */
    status = bind(server_socket, (struct sockaddr *)&server_address,
                  sizeof(server_address));

    if (status < 0) {
        printf("Error binding socket\n");
        exit(1);
    }

    printf("Socket bind successful\n");

    /* Start listening */
    status = listen(server_socket, 5);

    if (status < 0) {
        printf("Error listening socket\n");
        exit(1);
    }
    printf("Started listening\n");
    return server_socket;
}

int main() {
    char *input;       /* socket input message */
    int server_socket; /* socket for sending */
    int client_socket; /* socket used for receiving */
    ssize_t len;       /* string length */

    /* allocate memory for input */
    input = calloc(MAX_MSG, sizeof(char));

    server_socket = init_server_socket();

    printf("Waiting for connection\n");

    client_socket = open_client_socket(server_socket);
    printf("Accepted socket\n");

    /* Accept requests until interrupted */
    while (1) {
        /* Read input */
        len = recv(client_socket, input, MAX_MSG, 0);

        if (len < 0) {
            printf("Connection error\n");
            break;
        } else if (len == 0) {
            /* close client socket if received nothing */
            printf("Transmission complete. Close socket.\n");
            close(client_socket);
            client_socket = open_client_socket(server_socket);
            continue;
        }

        /* Check if input is a handshake initiation */
        if (strcmp(input, "AHOY") == 0) {
            handshake(client_socket);
            continue;
        }

        /* Write input to chat log */
        command_write(input);

        /* Parse message if input begins with colon */
        if (input[0] == ':') {
            char *tokenizable = strdup(input);
            chatMessage *message = parse_message(tokenizable);
            printf("<%s> %s\n", message->nickname, message->message);

            message->token = ""; /* Clear the token from the message */
            char *formatted_msg = format_message(message);
            send(client_socket, formatted_msg, strlen(formatted_msg), 0);
        }

        /* Clear the input array */
        memset(input, 0, MAX_MSG);
    }

    /* free memory for input */
    free(input);

    /* Close the sockets */
    close(client_socket);
    close(server_socket);

    return 0;
}
