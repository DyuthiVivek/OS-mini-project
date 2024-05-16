#include <stdio.h>
#include "client.h"
#include <string.h>
#include <unistd.h>

void print_user_menu(){
    printf("************************************\n");
    printf("1. View books\n");
    printf("2. Request to borrow a book\n");
    printf("3. Return a book\n");
    printf("4. Logout\n");
    printf("************************************\n\n");

    printf("Enter choice: ");
}


void view_books(int sock) {
    char buffer[4096]; // Increase buffer size to handle larger data

    printf("Books available:\n");

    // Receive the entire message from the server
    read(sock, buffer, sizeof(buffer));
    printf("%s", buffer);

}


void handle_user(int sock){
    int choice;
    while(1){
        print_user_menu();
        scanf("%d", &choice);
        getchar();
        switch(choice){
            case 1:
                write(sock, "1", 1);
                view_books(sock);

                break;
        }
    }
}