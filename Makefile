CC = clang
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

$(BIN_DIR)/client: $(SRC_DIR)/client.c
	$(CC) -o $(BIN_DIR)/client $(SRC_DIR)/client.c

$(BIN_DIR)/server: $(SRC_DIR)/server.c
	$(CC) -o $(BIN_DIR)/server $(SRC_DIR)/server.c
