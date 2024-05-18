#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/client.h"
#include "../include/server_admin.h"
#include "../include/client_user.h"

void print_admin_menu(){

    printf("************************************\n");
    printf("1. Add book\n");
    printf("2. Delete book\n");
    printf("3. Modify book\n");
    printf("4. Search for book\n");
    printf("5. Modify user details\n");
    printf("6. Lend book to user\n");
    printf("7. Accept book return from user\n");
    printf("8. View books\n");
    printf("9. Logout\n");
    printf("************************************\n\n");

    printf("Enter choice: ");
}

void send_book_details(int sock) {
    char name[MAX_BOOK_SIZE], author[MAX_AUTHOR_SIZE];
    char buffer[MAX_BOOK_SIZE + MAX_AUTHOR_SIZE + 7];
    int num_books;

    printf("Enter name of the book: ");
    fgets(name, MAX_BOOK_SIZE, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove newline character

    printf("Enter author of the book: ");
    fgets(author, MAX_AUTHOR_SIZE, stdin);
    author[strcspn(author, "\n")] = '\0'; // Remove newline character

    printf("Enter number of books available: ");
    scanf("%d", &num_books);
    getchar(); // Consume newline character left in the input buffer

    // Prepare message to send to server
    sprintf(buffer, "%s:%s:%d:", name, author, num_books);

    write(sock, buffer, strlen(buffer));
    // Send message to server
}

void send_user_details(int sock) {
    char username[MAX_NAME_LENGTH], phone[MAX_PHONE_LENGTH], password[MAX_PWD_LENGTH];
    char buffer[MAX_NAME_LENGTH + MAX_PHONE_LENGTH + MAX_PWD_LENGTH + 3]; // Assuming a reasonable buffer size

    printf("Enter username: ");
    fgets(username, MAX_NAME_LENGTH, stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove newline character

    printf("Enter phone: ");
    fgets(phone, MAX_PHONE_LENGTH, stdin);
    phone[strcspn(phone, "\n")] = '\0'; // Remove newline character

    printf("Enter password: ");
    fgets(password, MAX_PWD_LENGTH, stdin);
    password[strcspn(password, "\n")] = '\0'; // Remove newline character

    // Prepare message to send to server
    sprintf(buffer, "%s:%s:%s:", username, phone, password);

    // Send message to server
    write(sock, buffer, strlen(buffer));
}


void send_book(int sock) {
    char name[MAX_BOOK_SIZE];
    char buffer[MAX_BOOK_SIZE + 2];

    printf("Enter name of the book: ");
    fgets(name, MAX_BOOK_SIZE, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove newline character

    // Prepare message to send to server
    sprintf(buffer, "%s:", name);

    // Send message to server
    write(sock, buffer, strlen(buffer));
}

void send_borrow_details(int sock){
    char name[MAX_BOOK_SIZE], username[MAX_NAME_LENGTH];
    char buffer[MAX_NAME_LENGTH + MAX_BOOK_SIZE + 2];

    printf("Enter username: ");
    fgets(username, MAX_NAME_LENGTH, stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove newline character

    printf("Enter name of the book: ");
    fgets(name, MAX_BOOK_SIZE, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove newline character

    sprintf(buffer, "%s:%s:", username, name);
    write(sock, buffer, strlen(buffer));
}

void handle_admin(int sock){
    char recv[1];
    int choice;
    int exit = 0;
    while(!exit){
        print_admin_menu();
        scanf("%d", &choice);
        getchar();
        switch(choice){
            case 1:
                write(sock, "1", 1); // Send choice "1" to the server
                send_book_details(sock);

                read(sock, recv, 1);
                if((recv[0] - '0') == 1){
                    printf("Added book!\n");
                }
                else{
                    printf("Unable to add\n");
                }
                break;
            case 2:
                write(sock, "2", 1); // Send choice "2" to the server
                send_book(sock);
                read(sock, recv, 1);
                if((recv[0] - '0') == 2){
                    printf("Deleted book!\n");
                }
                else{
                    printf("Book not found\n");
                }
                break;
            case 3:
                write(sock, "3", 1); // Send choice "3" to the server

                send_book_details(sock);
                read(sock, recv, 1);

                if((recv[0] - '0') == 1){
                    printf("Updated book!\n");
                }
                else{
                    printf("Unable to update\n");
                }
                break;

            case 4:
                write(sock, "4", 1); // Send choice "4" to the server

                send_book(sock);
                read(sock, recv, 1);

                if((recv[0] - '0') == 1){

                    printf("Book found!\n");
                }
                else{
                    printf("Book not found\n");
                }
                break;
            case 5:
                write(sock, "5", 1); // Send choice "5" to the server
                send_user_details(sock);

                read(sock, recv, 1);

                if((recv[0] - '0') == 1){
                    printf("User details modified!\n");
                }
                else{
                    printf("User not found\n");
                }
                break;
            case 6:
                write(sock, "6", 1);
                send_borrow_details(sock);

                read(sock, recv, 1);

                if((recv[0] - '0') == 0){
                    printf("Book borrowed!\n");
                }
                else if((recv[0] - '0') == 1){
                    printf("User not found\n");
                }
                else if((recv[0] - '0') == 2){
                    printf("Book not available\n");
                }
                break;
            case 7:
                write(sock, "7", 1);
                send_borrow_details(sock);

                read(sock, recv, 1);

                if((recv[0] - '0') == 0){
                    printf("Book returned!\n");
                }
                else if((recv[0] - '0') == 1){
                    printf("Book not borrowed\n");
                }
                else if((recv[0] - '0') == 2){
                    printf("Error while updating\n");
                }
                break;

            case 8:
                write(sock, "8", 1);
                view_books(sock);
                break;
            case 9:
                write(sock, "9", 1);
                printf("Quitting\n");
                exit = 1;
                break;
            default:
                break;
        }
    }
}

