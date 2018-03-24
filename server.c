#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define MAX_MSG 512

char nick[32];

enum msg_type {
    MSG_ERROR = -1,
    MSG_NORMAL = 1,
    MSG_COMMAND = 2,
    MSG_HANDSHAKE = 4,
};

enum cmd_type {
    CMD_READ = 1,
    CMD_WRITE = 2,
    CMD_CLEAR = 4,
};

/*
Protocol handshake:
Client sends
AHOY
Server responds
AHOY-HOY:[random access token]

The client must store the token and include it in subsequent requests.

*/

/*
Structure of a message body is as follows:
:token:nickname:this is the message text
*/

struct Message {
    int token_offset;
    int token_length;
    int nick_offset;
    int nick_length;
    int message_offset;
    int message_length;
};

char *create_token(char *token, size_t size) {
    token = "12345678";
    return token;
}

int handshake(int socket) {
    printf("handshake()\n");
    /* */
    char *token;
    char *buf;
    size_t len;

    token = malloc(9);
    len = snprintf(NULL, 0, "AHOY-HOY:%s", create_token(token, 8));
    buf = (char *)malloc(len + 1);
    memset(buf, 0, len + 1);
    snprintf(buf, len + 1, "AHOY-HOY:%s", create_token(token, 8));
    printf("buf:%s\n", buf);

    send(socket, buf, strlen(buf), 0);
    return 0;
}

/* Parse message structure */
char **parse_message(char *message, int length) {
    char **tokens;
    char *token;
    char *tmp = message;
    int c;
    size_t count = 0;
    /* Parse the message and determine its type */
    printf("parse_message()\n");

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

    tokens = malloc(sizeof(char *) * count);

    c = 0;
    /* get the first token */
    token = strtok(message, ":");

    while (token != NULL) {
        printf("%d: %s\n", c, token);
        /* duplicate token into tokens */
        *(tokens + c++) = strdup(token);
        c++;
        token = strtok(NULL, ":");
    }

    return tokens;
}

int command_write(char *message) {
    FILE *fp;
    /* open file pointer */
    fp = fopen("chat.txt", "a+");
    /* append message to file */
    fputs(message, fp);
    fputs("\n", fp);
    /* close file pointer */
    fclose(fp);

    return 0;
}

int command_history(int socket) {
    FILE *fp;
    char *header;
    char buffer[1024];

    header = "Chat history:\n-------------\n";

    /* Open file pointer */
    fp = fopen("chat.txt", "r");
    /* Read file into buffer */
    fread(buffer, sizeof(buffer), 1, fp);
    /* send header into socket */
    send(socket, header, strlen(header), 0);
    /* send buffer into socket */
    send(socket, buffer, strlen(buffer), 0);
    return 0;
}

int main() {
    char *input;                       /* socket input message */
    struct sockaddr_in server_address; /* server socket parameters */
    int server_socket;                 /* socket for sending */
    int client_socket;                 /* socket used for receiving */
    int status;                        /* status code for function returns */
    ssize_t len;                       /* string length */
    int input_len;                     /* string length */
    char **message;

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
    input = malloc(MAX_MSG);

    printf("Waiting for connection\n");
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        printf("Unable to accept coonection on socket\n");
        exit(1);
    }
    printf("Accepted socket\n");

    /* Accept requests until interrupted */
    while (1) {
        printf("while(1)\n");
        /* Read input */
        len = recv(client_socket, input, MAX_MSG, 0);

        if (len < 0) {
            printf("Connection error\n");
            break;
        } else if (len == 0) {
            /* close client socket if received nothing */
            printf("Transmission complete. Close socket.\n");
            close(client_socket);
            continue;
        }

        printf("input:\n%s\n", input);

        /* Store the length of the input string */
        input_len = strlen(input);

        if (strcmp(input, "AHOY") == 0) {
            handshake(client_socket);
            continue;
        }

        message = parse_message(input, input_len);

        printf("message[0]: %s\n", message[0]);
        command_write(input);
        send(client_socket, "foo", strlen("foo"), 0);
        printf("Response sent\n");
    }

    /* free memory for input */
    free(input);

    /* Close the sockets */
    close(client_socket);
    close(server_socket);

    return 0;
}
