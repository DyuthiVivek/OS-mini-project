CC = gcc
CFLAGS = -Wall -Wextra

# Client compilation
client: client.c client_admin.c client_login.c client_user.c
	$(CC) $(CFLAGS) -o client client.c client_admin.c client_login.c client_user.c

# Server compilation
server: server.c server_admin.c server_handle_login.c server_user.c locking.c
	$(CC) $(CFLAGS) -o server server.c server_admin.c server_handle_login.c server_user.c locking.c

.PHONY: clean

clean:
	rm -f client server
