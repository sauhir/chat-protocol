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

/* Fix utime() warning on Linux */
#define _BSD_SOURCE

#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef __linux
#include <sys/select.h>
#endif

#include "chat.h"
#include "chat_message.h"
#include "server.h"
#include "server_commands.h"
#include "session.h"

#define PORT 8002
#define MAX_SOCKETS 10

static volatile int running;

/*
Protocol handshake:
Client sends
AHOY
Server responds
AHOY-HOY:[random access token]

The client must store the token and include it in subsequent requests.
*/

void interrupt_handler() {
    running = 0;
}

/*
 * Send handshake response.
 */
int handshake(int socket) {
    char *token;
    char *buf;
    size_t len;

    token = (char *)calloc(9, sizeof(char));
    len = snprintf(NULL, 0, "AHOY-HOY:%s", create_token(token, 8));
    buf = (char *)calloc(len + 1, sizeof(char));
    memset(buf, 0, len + 1);
    snprintf(buf, len + 1, "AHOY-HOY:%s", create_token(token, 8));

    printf("Send handshake response\n");
    send(socket, buf, strlen(buf), 0);
    free(token);
    free(buf);
    return 0;
}

/*
 * Open client socket connection
 */
int open_client_socket(int server_socket) {
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        printf("Unable to accept connection on socket\n");
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
    server_address.sin_port = htons(PORT);
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
    printf("Started listening...\n");
    return server_socket;
}

/*
 * Iterate through the array of available sockets to find an unused socket
 * Returns the free socket array index
 * Returns -1 if no free socket is available
 */
int get_free_socket_idx(int *sockets) {
    int i;
    for (i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i] == -1) {
            return i;
        }
    }
    return -1;
}

void send_to_socket(int socket, char *message) {
    if (socket) {
        send(socket, message, strlen(message), 0);
    }
}

void send_to_all(int *sockets, int max_sockets, char *message) {
    int i;
    for (i = 0; i < max_sockets; i++) {
        if (sockets[i]) {
            send(sockets[i], message, strlen(message), 0);
        }
    }
}

int main(int argc, char *argv[]) {
    char *input;                     /* socket input message */
    int server_socket;               /* socket for sending */
    ssize_t len;                     /* string length */
    fd_set socket_set;               /* file descriptor set */
    struct timeval select_timeout;   /* select() timeout */
    struct timeval sr_timeout;       /* send() recv() timeout */
    int max_descriptor;              /* hold the largest fd number */
    int client_sockets[MAX_SOCKETS]; /* array to hold client sockets */
    long seconds;
    int i;

    signal(SIGINT, interrupt_handler);

    select_timeout.tv_sec = 1;
    select_timeout.tv_usec = 0;

    sr_timeout.tv_sec = 5;
    sr_timeout.tv_usec = 0;

    /* allocate memory for input */
    input = calloc(MAX_MSG, sizeof(char));

    server_socket = init_server_socket();
    max_descriptor = server_socket;

    running = 1;

    /* Invalidate initial socket list */
    for (i = 0; i < MAX_SOCKETS; i++) {
        client_sockets[i] = -1;
    }

    seconds = time(NULL);

    /* Accept requests until interrupted */
    while (running) {
        chatMessage *message;
        char *formatted_msg;

        /* Clear input buffer */
        memset(input, 0, MAX_MSG);

        /* Reset the file descriptors */
        FD_ZERO(&socket_set);
        FD_SET(STDIN_FILENO, &socket_set);
        FD_SET(server_socket, &socket_set);
        for (i = 0; i < MAX_SOCKETS; i++) {
            if (client_sockets[i] >= 0) {
                FD_SET(client_sockets[i], &socket_set);
            }
        }

        if ((time(NULL) - seconds) > 15) {
            printf("Ping!\n");
            send_to_all(client_sockets, MAX_SOCKETS, "::server:ping:PING\n");
            seconds = time(NULL);
        }

        if (select(max_descriptor + 1, &socket_set, NULL, NULL,
                   &select_timeout)) {
            /* Check input from stdin */
            if (FD_ISSET(0, &socket_set)) {
                printf("\nShutting down\n");
                running = 0;
                continue;
            }
            /* Check connection attempt to server socket */
            if (FD_ISSET(server_socket, &socket_set)) {
                int client_socket;
                int free_socket_idx;
                free_socket_idx = get_free_socket_idx(client_sockets);
                if (free_socket_idx < 0) {
                    printf("No free socket available\n");
                    continue;
                } else {
                    printf("Free socket at index %d\n", free_socket_idx);
                }

                printf("Request on socket: %d\n", server_socket);
                client_socket = open_client_socket(server_socket);
                setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO,
                           (const char *)&sr_timeout, sizeof sr_timeout);
                setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO,
                           (const char *)&sr_timeout, sizeof sr_timeout);

                printf("Client connected: %d\n", client_socket);
                client_sockets[free_socket_idx] = client_socket;
                if (client_socket > max_descriptor) {
                    max_descriptor = client_socket;
                }
            }
            /* Check connections to client sockets */
            for (i = 0; i < MAX_SOCKETS; i++) {
                if (client_sockets[i] == -1) {
                    continue;
                }

                if (FD_ISSET(client_sockets[i], &socket_set)) {
                    printf("Client socket activity: %d\n", i);
                    len = recv(client_sockets[i], input, MAX_MSG, 0);

                    printf("len: %ld\n", len);

                    if (len <= 0) {
                        /* Handle closed connection */
                        close(client_sockets[i]);
                        client_sockets[i] = -1;

                        send_to_all(client_sockets, MAX_SOCKETS,
                                    "::server:status:Somebody left the chat\n");
                    } else if (strcmp(input, "AHOY") == 0) {
                        /* If handshake init detected, shake hands */
                        handshake(client_sockets[i]);

                        send_to_all(client_sockets, MAX_SOCKETS,
                                    "::server:status:"
                                    "Somebody joined the chat\n");
                    } else if (input[0] == ':') {
                        /* Parse message if input begins with colon */
                        command_write(input);
                        message = parse_message(input);
                        printf("%s\n", input);
                        /* Clear the token from the message */
                        message->token = "";
                        formatted_msg = format_message(message);
                        /* Send message back to all clients */
                        send_to_all(client_sockets, MAX_SOCKETS, formatted_msg);
                        free(formatted_msg);
                        free(message);
                    }
                }
            }
        } else {
            /* No input received */
        }
        usleep(20000);
    }

    free(input);

    /* Close the sockets */
    printf("Close sockets\n");
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (client_sockets[i] >= 0) {
            close(client_sockets[i]);
        }
    }
    close(server_socket);

    return 0;
}
