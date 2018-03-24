CC=clang
MKDIR_P = mkdir -p
OUT_DIR = bin
SRC_DIR = src

.PHONY: directories

all: directories bin/client bin/server

directories: $(OUT_DIR) $(SRC_DIR)

$(OUT_DIR):
	$(MKDIR_P) $(OUT_DIR)

$(SRC_DIR):
	$(MKDIR_P) $(SRC_DIR)

bin/client:
	$(CC) -o $(OUT_DIR)/client $(SRC_DIR)/client.c

bin/server:
	$(CC) -o $(OUT_DIR)/server $(SRC_DIR)/server.c
