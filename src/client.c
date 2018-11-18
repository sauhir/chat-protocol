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

#include <curses.h>

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <locale.h>
#include <netinet/in.h>
#include <signal.h>
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
#include "client.h"
#include "session.h"

static volatile int running;

static WINDOW *mainwindow;
static WINDOW *inputwindow;

int printtime();
void curses_init();

void interrupt_handler(int signal) {
    if (signal == SIGINT) {
        running = 0;
    }
}

int printtime(void) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    wprintw(mainwindow, "[%02d:%02d:%02d] ", timeinfo->tm_hour,
            timeinfo->tm_min, timeinfo->tm_sec);

    return 0;
}

int handshake(int socket, chatSession *session) {
    char *response;
    char *part;
    char *token;

    response = calloc(1, MAX_MSG);

    /* send greeting */
    send(socket, "AHOY", MAX_MSG, 0);
    printtime();
    wprintw(mainwindow, "Greeting sent\n");

    /* receive response */
    recv(socket, response, MAX_MSG, 0);

    /* get first part of response */
    part = strtok(response, ":");

    if (strcmp(part, "AHOY-HOY") == 0) {
        printtime();
        wprintw(mainwindow, "Correct response received\n");
        /* Response correct, get token */
        part = strtok(NULL, ":");
        token = calloc(1, strlen(part));
        strcpy(token, part);
        session->token = token;
        wprintw(mainwindow, "Authentication token: %s\n", session->token);
        free(response);
    } else {
        free(response);
        return -1;
    }
    return 0;
}

/*
 * Prompt for nickname and store it into the session.
 */
int set_nickname(chatSession *session) {
    char *nick;
    unsigned int i;

    nick = calloc(MAX_NICK, sizeof(char));
    wmove(inputwindow, 0, 0);
    wprintw(inputwindow, "Select a nickname:\n");
    wrefresh(mainwindow);
    wrefresh(inputwindow);
    wscanw(inputwindow, " %99[^\n]", nick);

    /* Replace colons with underscore in nickname */
    for (i = 0; i < strlen(nick); i++) {
        if (nick[i] == ':' || nick[i] == ' ') {
            nick[i] = '_';
        }
    }

    session->nickname = nick;
    return 0;
}

int init_socket(char *address) {
    struct sockaddr_in server_address;
    int network_socket;
    int status;

    /* Create a socket */
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Specify an address for the socket */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8002);
    server_address.sin_addr.s_addr = inet_addr(address);

    /* Connect to the server */
    status = connect(network_socket, (struct sockaddr *)&server_address,
                     sizeof(server_address));

    /* Check for connection error */
    if (status == -1) {
        wprintw(mainwindow,
                "There was an error connecting to the remote socket.\n"
                "Press any key to quit.\n");
        getch();
        endwin();
        exit(1);
    }
    return network_socket;
}

/*
 * Create and return a new curses window
 */
WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    wrefresh(local_win);
    return local_win;
}

void send_message(chatSession *session, char *buffer, int network_socket) {
    char *msg_str;
    chatMessage *message = malloc(sizeof(chatMessage));
    message->token = session->token;
    message->nickname = session->nickname;
    message->message_type = msg_type_normal;
    message->message = buffer;

    msg_str = format_message(message);
    /* Send message to server */
    send(network_socket, msg_str, MAX_MSG, 0);
    free(msg_str);
}

/*
 * Initialize curses windows
 */
void curses_init(void) {
    setlocale(LC_ALL, "");
    initscr();
    mainwindow = create_newwin(LINES - 3, COLS, 0, 0);
    inputwindow = create_newwin(3, COLS, LINES - 3, 0);
    scrollok(mainwindow, TRUE);
    scrollok(inputwindow, TRUE);
}

/*
 * Clear the input field and reset the input position.
 */
void reset_input(char *input_buffer, int *input_pos, chatSession *session) {
    memset(input_buffer, 0, MAX_MSG);
    *input_pos = 0;
    wclear(inputwindow);
    wprintw(inputwindow, "%s >> ", session->nickname);
}

int main(int argc, char *argv[]) {
    int network_socket;    /* Socket for the server connection */
    char *server_response; /* Server response buffer */
    char *input_buffer;    /* Input buffer */
    char *address;         /* IP address of the server */
    chatSession *session;  /* Current chat session */

    size_t len;                    /* Server response length */
    int input_pos = 0;             /* Current input cursor position */
    fd_set socket_set;             /* file descriptor set */
    struct timeval select_timeout; /* select() timeout */
    struct timeval recv_timeout;   /* recv() timeout */

    running = 1;
    /* Set a 10 msec timeout to select() calls */
    select_timeout.tv_sec = 0;
    select_timeout.tv_usec = 10000;

    /* Set a 3 second timeout to recv() calls */
    recv_timeout.tv_sec = 3;
    recv_timeout.tv_usec = 0;

    /* Get server IP from command line arg if available */
    if (argc == 1) {
        address = "127.0.0.1";
    } else {
        address = argv[1];
    }

    curses_init();

    /* Add interrupt handler to catch CTRL-C */
    signal(SIGINT, interrupt_handler);

    wprintw(mainwindow, "Connecting to server %s\n", address);
    wrefresh(mainwindow);

    network_socket = init_socket(address);

    setsockopt(network_socket, SOL_SOCKET, SO_RCVTIMEO,
               (const char *)&recv_timeout, sizeof recv_timeout);

    /* Initialize the session variable */
    session = create_session();

    printtime();
    wprintw(mainwindow, "Connected to the server\n");
    wrefresh(mainwindow);

    server_response = calloc(MAX_MSG, sizeof(char));
    input_buffer = calloc(MAX_MSG, sizeof(char));

    /* Shake hands with the server */
    if (handshake(network_socket, session) == -1) {
        wprintw(mainwindow, "Handshake failed.\n");
        endwin();
        free(server_response);
        free(input_buffer);
        exit(1);
    }

    /* Select nick for user */
    set_nickname(session);

    nodelay(inputwindow, TRUE);
    noecho();

    reset_input(input_buffer, &input_pos, session);

    /* Main loop */
    while (running) {
        int c, c2, x, y;
        chatMessage *msg;

        wrefresh(mainwindow);
        wrefresh(inputwindow);

        /* Reset the file descriptors */
        FD_ZERO(&socket_set);
        FD_SET(network_socket, &socket_set);

        /* Read a character from the inputwindow */
        c = wgetch(inputwindow);

        /* Character code logging */
        /*
        if (c > 0) {
            wprintw(mainwindow, "%ld\n", c);
        }*/

        if (c == 195) {
            /* First byte of scandinavian character */
            /* Read second byte and print both */
            c2 = wgetch(inputwindow);
            wprintw(inputwindow, "%c%c", c, c2);
            input_buffer[input_pos++] = c;
            input_buffer[input_pos++] = c2;
        } else if (c == '\033') {
            /* If escape code received, supress two next characters */
            wgetch(inputwindow);
            wgetch(inputwindow);
        } else if (c == 127 || c == 8) {
            if (input_pos > 0) {
                /* Backspace was pressed. Erase the last character.*/
                input_buffer[--input_pos] = 0;
                getyx(inputwindow, y, x);
                mvwaddch(inputwindow, y, x - 1, ' ');
                wmove(inputwindow, y, x - 1);
            }
        } else if (c == 13 || c == 10) {
            /* If a newline is reached submit the contents of input_buffer */
            if (input_pos == 0) {
                continue;
            }
            if (strcmp(input_buffer, "/quit") == 0) {
                running = 0;
                continue;
            }
            if (strncmp(input_buffer, "/nick", 5) == 0) {
                if (strlen(input_buffer) > 7) {
                    char nick_tmp[MAX_NICK];
                    memcpy(nick_tmp, &input_buffer[6],
                           strlen(input_buffer) - 6);
                    session_replace_nick(session, nick_tmp);
                    reset_input(input_buffer, &input_pos, session);
                }
                continue;
            }
            send_message(session, input_buffer, network_socket);
            reset_input(input_buffer, &input_pos, session);
        } else if (c >= 32) {
            wprintw(inputwindow, "%c", c);
            input_buffer[input_pos++] = c;
        }

        if (select(network_socket + 1, &socket_set, NULL, NULL,
                   &select_timeout)) {
            if (FD_ISSET(network_socket, &socket_set)) {
                if (!running) {
                    break;
                }
                /* Get response from server */
                len = recv(network_socket, server_response, MAX_MSG, 0);

                if (len <= 0) {
                    running = 0;
                    break;
                }

                msg = parse_message(server_response);

                /* Print out if valid message */
                if (msg != NULL) {
                    if (msg->message_type == msg_type_status) {
                        printtime();
                        wprintw(mainwindow, "-- %s\n", msg->message);
                    } else if (msg->message_type == msg_type_normal) {
                        printtime();
                        wprintw(mainwindow, "<%s> %s\n", msg->nickname,
                                msg->message);
                    }
                }
                free(msg);
            }
        }

        /* Clear the arrays */
        memset(server_response, 0, MAX_MSG);
    }

    /* Curses cleanup */
    delwin(mainwindow);
    delwin(inputwindow);
    endwin();

    free(server_response);
    free(input_buffer);
    free(session);

    /* Close the socket */
    close(network_socket);

    return 0;
}
