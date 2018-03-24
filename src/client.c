#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_RESPONSE 512

char nick[32];
char token[8];

int printtime() {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("\n[%02d:%02d:%02d] ", timeinfo->tm_hour, timeinfo->tm_min,
                 timeinfo->tm_sec);

    return 0;
}

int handshake(int socket) {
    char response[MAX_RESPONSE];
    char *part;

    /* send greeting */
    send(socket, "AHOY", MAX_RESPONSE, 0);
    /* receive response */
    printtime();
    printf("Greeting sent\n");

    recv(socket, response, MAX_RESPONSE, 0);

    /* get first part of response */
    part = strtok(response, ":");

    if (strcmp(part, "AHOY-HOY") == 0) {
        printtime();
        printf("Correct response received\n");
        /* Response correct, get token */
        part = strtok(NULL, ":");
        printf("Authentication token: %s\n", part);
    } else {
        return -1;
    }
    return 0;
}

int main() {
    struct sockaddr_in server_address;
    int network_socket;
    int status;
    ssize_t len;

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

    printtime();
    printf("Connected to the server\n");

    server_response = malloc(MAX_RESPONSE);
    input = malloc(MAX_RESPONSE);

    status = handshake(network_socket);
    if (status == -1) {
        printtime();
        printf("Handshake failed.\n");
        exit(1);
    }

    while (1) {
        printf("while(1)\n");

        printf("Say something:\n");
        scanf(" %99[^\n]", input);
        // scanf("%s", input);
        // input = "foo";
        printf("You wrote: %s\n", input);
        /* Send message to server */
        len = send(network_socket, input, MAX_RESPONSE, 0);
        printf("sent %ld bytes", len);
        /* Get response from server */
        len = recv(network_socket, server_response, MAX_RESPONSE, 0);
        printf("received %ld bytes", len);
        /* Print out the response */
        printtime();
        printf("The server says: %s\n", server_response);

        server_response = malloc(MAX_RESPONSE);
        input = malloc(MAX_RESPONSE);
    }

    /* Close the socket */
    close(network_socket);

    return 0;
}
