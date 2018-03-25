
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>

/*
 * Display chat log
 */
int command_history(int socket) {
    FILE *fp;
    char *header;
    char buffer[1024];

    header = "Chat history:\n-------------\n";

    fp = fopen("chat.txt", "r");
    fread(buffer, sizeof(buffer), 1, fp);
    send(socket, header, strlen(header), 0);
    send(socket, buffer, strlen(buffer), 0);
    return 0;
}

/*
 * Write message into chat log file
 */
int command_write(char *message) {
    FILE *fp;
    fp = fopen("chat.txt", "a+");
    fputs(message, fp);
    fputs("\n", fp);
    fclose(fp);

    return 0;
}