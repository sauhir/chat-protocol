CC = gcc
MKDIR_P = mkdir -p
BIN_DIR = bin
SRC_DIR = src

.PHONY: directories

all: directories bin/client bin/server

directories: $(BIN_DIR) $(SRC_DIR)

$(BIN_DIR):
	$(MKDIR_P) $(BIN_DIR)

$(SRC_DIR):
	$(MKDIR_P) $(SRC_DIR)

$(BIN_DIR)/client: $(SRC_DIR)/client.c $(SRC_DIR)/chat_message.c $(SRC_DIR)/session.c $(SRC_DIR)/error_log.c
	$(CC) -o $(BIN_DIR)/client \
	-Wall -Wextra -std=c89 -O2 \
	$(SRC_DIR)/client.c $(SRC_DIR)/chat_message.c $(SRC_DIR)/session.c \
	$(SRC_DIR)/error_log.c \
	-lcurses

$(BIN_DIR)/server: $(SRC_DIR)/server.c $(SRC_DIR)/server_commands.c $(SRC_DIR)/chat_message.c $(SRC_DIR)/session.c $(SRC_DIR)/error_log.c
	$(CC) -o $(BIN_DIR)/server \
	-Wall -Wextra -std=c89 -O2 \
	$(SRC_DIR)/server.c $(SRC_DIR)/server_commands.c \
	$(SRC_DIR)/chat_message.c $(SRC_DIR)/session.c \
	$(SRC_DIR)/error_log.c
