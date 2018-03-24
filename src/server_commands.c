
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

/*
 * Write message into chat log file
 */
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