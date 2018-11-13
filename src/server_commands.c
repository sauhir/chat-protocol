/* Server commands
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
#include <sys/socket.h>
#include <unistd.h>

#include "server_commands.h"

/*
 * Display chat log
 */
int command_history(int socket) {
    FILE *fp;
    char str[80];

    fp = fopen("chat.txt", "r");

    while (fgets(str, 80, fp) != NULL) {
        send(socket, str, strlen(str), 0);
        /* Add delay because without delay the client would receive only the
           first message */
        usleep(20000);
    }

    fclose(fp);
    return 0;
}

/*
 * Write message into chat log file
 */
int command_write(char *message) {
    FILE *fp;
    fp = fopen("chat.txt", "a+");
    fputs(message, fp);
    fclose(fp);

    return 0;
}
