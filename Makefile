CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
BIN_DIR = bin

# Client compilation
client: $(SRC_DIR)/client.c $(SRC_DIR)/client_admin.c $(SRC_DIR)/client_login.c $(SRC_DIR)/client_user.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client $(SRC_DIR)/client.c $(SRC_DIR)/client_admin.c $(SRC_DIR)/client_login.c $(SRC_DIR)/client_user.c $(SRC_DIR)/locking.c

# Server compilation
server: $(SRC_DIR)/server.c $(SRC_DIR)/server_admin.c $(SRC_DIR)/server_login.c $(SRC_DIR)/server_user.c $(SRC_DIR)/locking.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $(SRC_DIR)/server.c $(SRC_DIR)/server_admin.c $(SRC_DIR)/server_login.c $(SRC_DIR)/server_user.c $(SRC_DIR)/locking.c

.PHONY: clean

clean:
	rm -f $(BIN_DIR)/client $(BIN_DIR)/server
