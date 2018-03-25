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

/*
Protocol handshake:
Client sends
AHOY
Server responds
AHOY-HOY:[random access token]

The client must store the token and include it in subsequent requests.
*/

/*
 * Generate an access token for the client
 */
char *create_token(char *token, size_t size) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char *random_string = NULL;

    random_string = calloc((size + 1), sizeof(char));

    if (random_string) {
        for (int n = 0; n < size; n++) {
            int key = rand() % (int)(sizeof(charset) - 1);
            random_string[n] = charset[key];
        }

        random_string[size] = '\0';
    }

    token = random_string;
    return random_string;
}

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

int open_client_socket(int server_socket) {
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        printf("Unable to accept coonection on socket\n");
        exit(1);
    }
    return client_socket;
}

int main() {
    char *input;                       /* socket input message */
    struct sockaddr_in server_address; /* server socket parameters */
    int server_socket;                 /* socket for sending */
    int client_socket;                 /* socket used for receiving */
    int status;                        /* status code for function returns */
    ssize_t len;                       /* string length */

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

    /* allocate memory for input */
    input = calloc(MAX_MSG, sizeof(char));

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

        printf("input:\n%s\n", input);

        /* Check if input is a handshake initiation */
        if (strcmp(input, "AHOY") == 0) {
            handshake(client_socket);
            continue;
        }

        command_write(input);
        char *tokenizable = strdup(input);

        chatMessage *message = parse_message(tokenizable);

        printf("<%s> %s\n", message->nickname, message->message);
        send(client_socket, input, strlen(input), 0);
        printf("Response sent\n");
    }

    /* free memory for input */
    free(input);

    /* Close the sockets */
    close(client_socket);
    close(server_socket);

    return 0;
}
