#include <stdio.h>
#include "../include/client.h"
#include <string.h>
#include <unistd.h>

void print_user_menu(){
    printf("************************************\n");
    printf("1. View books\n");
    printf("2. View borrowed books\n");
    printf("3. Logout\n");
    printf("************************************\n\n");

    printf("Enter choice: ");
}


void view_books(int sock) {
    char buffer[4096]; // Increase buffer size to handle larger data

    printf("Books:\n");

    // Receive the entire message from the server
    read(sock, buffer, sizeof(buffer));
    printf("%s", buffer);

}


void handle_user(int sock){
    int choice;
    int exit = 0;
    while(!exit){
        print_user_menu();
        scanf("%d", &choice);
        getchar();
        switch(choice){
            case 1:
                write(sock, "1", 1);
                view_books(sock);
                break;
            case 2:
                write(sock, "2", 1);
                view_books(sock);
                break;
            case 3:
                write(sock, "3", 1);
                printf("Quitting\n");
                exit = 1;
                break;
            default:
                printf("Invalid choice. Try again.\n");

        }
    }
}