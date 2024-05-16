#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server_admin.h"
#include "server_user.h"

#define PORT 8080
#define MAX_CLIENTS 100

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    int ret;

    int flag = 1;
    char buffer[1];
    char return_buffer[1];
    char message[MAX_NAME_LENGTH + MAX_PWD_LENGTH + 2];
    char message_create_user[MAX_NAME_LENGTH + MAX_PHONE_LENGTH + MAX_PWD_LENGTH + 3];
    char *token, *name, *password;

    while(flag){
        read(client_socket, buffer, 1);
        // printf("option: %d\n", buffer[0] - '0');
        int option = buffer[0] - '0';

        // authenicate admin or user
        if ((option == 1) || (option) == 2){
            read(client_socket, message, sizeof(message)); // Read combined message
            token = strtok(message, ":"); // Tokenize the message using separator
            name = token;
            token = strtok(NULL, ":");
            password = token;

            if (option == 1)
                ret = authenticate(name, password, ADMINS_FILE);
            else
                ret = authenticate(name, password, USERS_FILE);

            return_buffer[0] = ret + '0';
            
            write(client_socket, return_buffer, 1);

            if (ret == 0){
                if (option == 1)
                    handle_server_admin(client_socket, name);
                else
                    handle_server_user(client_socket, name);
                flag = 0;
            }

        }

        // create user
        else if (option == 3){

            read(client_socket, message_create_user, sizeof(message_create_user)); // Read combined message
            printf("read from client\n");

            char name[MAX_NAME_LENGTH];
            char phone[MAX_PHONE_LENGTH];
            char password[MAX_PWD_LENGTH];

            token = strtok(message_create_user, ":"); // Tokenize the message using separator
            if (token != NULL) {
                strncpy(name, token, MAX_NAME_LENGTH - 1);
                name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
            }
            token = strtok(NULL, ":");
            if (token != NULL) {
                strncpy(phone, token, MAX_PHONE_LENGTH - 1);
                phone[MAX_PHONE_LENGTH - 1] = '\0'; // Ensure null-termination
            }
            token = strtok(NULL, ":");
            if (token != NULL) {
                strncpy(password, token, MAX_PWD_LENGTH - 1);
                password[MAX_PWD_LENGTH - 1] = '\0'; // Ensure null-termination
            }

            // printf("calling create user\n");
            ret = create_user(name, phone, password);
            return_buffer[0] = ret + '0';
            write(client_socket, return_buffer, strlen(return_buffer));

        }

    }

    printf("One connection closed\n");
    close(client_socket);
    return NULL;
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;
    pthread_attr_t attr;

    // Initialize thread attributes
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Set threads to be detached

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept the incoming connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");

        // Create a new thread to handle the client with detached attribute
        if (pthread_create(&thread_id, &attr, handle_client, (void *)&client_socket) != 0) {
            perror("Thread creation failed");
            close(client_socket);
        }
    }

    // Destroy thread attributes
    pthread_attr_destroy(&attr);

    return 0;
}
