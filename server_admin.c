#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "server.h"
#include "server_admin.h"
#include "server_handle_login.h"
#include "locking.h"


void add_book(int client_socket) {
    Book new_book;
    int fd, ret;

    char buffer[MAX_BOOK_SIZE + MAX_AUTHOR_SIZE + 7]; // Assuming a reasonable buffer size
    read(client_socket, buffer, MAX_BOOK_SIZE + MAX_AUTHOR_SIZE + 7);

    // Tokenize the received message to extract book details
    char *token = strtok(buffer, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(new_book.name, token, MAX_BOOK_SIZE);

    token = strtok(NULL, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(new_book.author, token, MAX_AUTHOR_SIZE);

    token = strtok(NULL, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        exit(EXIT_FAILURE);
    }
    new_book.num_copies = atoi(token);
    new_book.deleted = 0;

    // Open the file with write permission and create if not exists
    if ((fd = open("books.bin", O_RDWR | O_CREAT, 0644)) == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // Acquire lock before writing the new record
    acquire_lock(fd, F_WRLCK);

    // Set book ID (assuming it's serially increasing)
    //lseek(fd, 0, SEEK_END);
    new_book.book_id = lseek(fd, 0, SEEK_END) / sizeof(Book) + 1;

    // Write the new book record to the file
    if ((ret = write(fd, &new_book, sizeof(new_book))) == -1) {
        perror("Error writing to file");
        release_lock(fd);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Release lock after writing
    release_lock(fd);
    close(fd);

    write(client_socket, "1", 1);

}

void delete_book(int client_socket) {
    int fd;
    char name[MAX_BOOK_SIZE];
    char buffer[MAX_BOOK_SIZE + 2]; // Adding 2 for ':' and null terminator

    int found = 0;


    // Read the name of the book to be deleted from the client
    read(client_socket, buffer, MAX_BOOK_SIZE + 2);

    char *token = strtok(buffer, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(name, token, MAX_BOOK_SIZE);

    // Open the file with read-write permission
    if ((fd = open("books.bin", O_RDWR)) == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // Acquire lock before reading and writing
    acquire_lock(fd, F_WRLCK);

    // Search for the book by name and mark it as deleted
    Book temp_book;
    while (read(fd, &temp_book, sizeof(Book)) > 0) {
        if (strcmp(temp_book.name, name) == 0 && temp_book.deleted != 1) {
            temp_book.deleted = 1; // Mark the book as deleted
            lseek(fd, -sizeof(Book), SEEK_CUR); // Move back to the beginning of the record
            if (write(fd, &temp_book, sizeof(Book)) == -1) {
                perror("Error updating book record");
                release_lock(fd);
                close(fd);
                exit(EXIT_FAILURE);
            }
            found = 1;
            break;
        }
    }

    // Release lock after reading and writing
    release_lock(fd);
    close(fd);

    // Send status back to client
    if (found) {
        write(client_socket, "2", 1);// Send '2' to indicate successful deletion
    } else {
        write(client_socket, "0", 1);// Send '0' to indicate book not found
    }
}

void search_book(int client_socket) {
    int fd;
    char name[MAX_BOOK_SIZE];
    char buffer[MAX_BOOK_SIZE + 2]; // Adding 2 for ':' and null terminator

    int found = 0;

    // Open the file with read-write permission
    if ((fd = open("books.bin", O_RDWR)) == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the name of the book to be deleted from the client
    read(client_socket, buffer, MAX_BOOK_SIZE + 2);
    char *token = strtok(buffer, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        release_lock(fd);
        close(fd);
        exit(EXIT_FAILURE);
    }
    strncpy(name, token, MAX_BOOK_SIZE);

    // Acquire lock before reading and writing
    acquire_lock(fd, F_RDLCK);

    // Search for the book by name and mark it as deleted
    Book temp_book;
    while (read(fd, &temp_book, sizeof(Book)) > 0) {
        if (strcmp(temp_book.name, name) == 0 && temp_book.deleted != 1) {
            found = 1;
            break;
        }
    }

    // Release lock after reading
    release_lock(fd);
    close(fd);

    // Send status back to client
    if (found) {
        write(client_socket, "1", 1);// Send '2' to indicate successful deletion
    } else {
        write(client_socket, "0", 1);// Send '0' to indicate book not found
    }
}

void update_book(int client_socket) {
    int fd_books;
    char name[MAX_BOOK_SIZE];
    char author[MAX_AUTHOR_SIZE];
    int num_copies;
    char buffer[MAX_BOOK_SIZE + MAX_AUTHOR_SIZE + 7]; // Assuming a reasonable buffer size

    int found = 0;

    // Open the main book file with read-write permission
    if ((fd_books = open("books.bin", O_RDWR)) == -1) {
        perror("Error opening main book file");
        exit(EXIT_FAILURE);
    }


    // Read the details of the book to be updated from the client
    read(client_socket, buffer, MAX_BOOK_SIZE + MAX_AUTHOR_SIZE + 7);

    // Tokenize the received message to extract book details
    char *token = strtok(buffer, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        close(fd_books);
        exit(EXIT_FAILURE);
    }
    strncpy(name, token, MAX_BOOK_SIZE);

    token = strtok(NULL, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        close(fd_books);
        exit(EXIT_FAILURE);
    }
    strncpy(author, token, MAX_AUTHOR_SIZE);

    token = strtok(NULL, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed book details received from client.\n");
        close(fd_books);
        exit(EXIT_FAILURE);
    }
    num_copies = atoi(token);

    // Acquire locks before reading and writing
    acquire_lock(fd_books, F_WRLCK);

    // Search for the book by name and update its details
    Book temp_book;
    off_t curr_pos_books = 0;
    while (read(fd_books, &temp_book, sizeof(Book)) > 0) {
        if (strcmp(temp_book.name, name) == 0 && temp_book.deleted != 1) {
            // Update book details
            strncpy(temp_book.name, name, MAX_BOOK_SIZE);
            strncpy(temp_book.author, author, MAX_AUTHOR_SIZE);
            temp_book.num_copies = num_copies;
            
            // Move the file pointer to the position of the current record
            lseek(fd_books, curr_pos_books, SEEK_SET);

            // Write the updated book record to the main book file
            if (write(fd_books, &temp_book, sizeof(Book)) == -1) {
                perror("Error updating book record");
                release_lock(fd_books);
                close(fd_books);
                exit(EXIT_FAILURE);
            }

            found = 1;
            break;
        }
        // Store the current position in case the book is found
        curr_pos_books = lseek(fd_books, 0, SEEK_CUR);
    }

  
    // Release locks after reading and writing
    release_lock(fd_books);
    close(fd_books);

    // Send status back to client
    if (found) {
        write(client_socket, "1", 1);// Send '1' to indicate successful update
    } else {
        write(client_socket, "0", 1);// Send '0' to indicate book not found
    }

 
}

void update_user(int client_socket) {
    int fd;
    char username[MAX_NAME_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char password[MAX_PWD_LENGTH];
    char buffer[MAX_NAME_LENGTH + MAX_PHONE_LENGTH + MAX_PWD_LENGTH + 3]; // Assuming a reasonable buffer size

    int found = 0;

    // Open the user file with read-write permission
    if ((fd = open("users.bin", O_RDWR)) == -1) {
        perror("Error opening user file");
        exit(EXIT_FAILURE);
    }

    // Read the details of the user to be updated from the client
    read(client_socket, buffer, MAX_NAME_LENGTH + MAX_PHONE_LENGTH + MAX_PWD_LENGTH + 3);

    // Tokenize the received message to extract user details
    char *token = strtok(buffer, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed user details received from client.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    strncpy(username, token, MAX_NAME_LENGTH);

    token = strtok(NULL, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed user details received from client.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    strncpy(phone, token, MAX_PHONE_LENGTH);

    token = strtok(NULL, ":");
    if (token == NULL) {
        fprintf(stderr, "Error: Malformed user details received from client.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    strncpy(password, token, MAX_PWD_LENGTH);

    // Acquire lock before reading and writing
    acquire_lock(fd, F_WRLCK);

    // Search for the user by username and update its details
    User temp_user;
    off_t curr_pos = 0;
    while (read(fd, &temp_user, sizeof(User)) > 0) {
        if (strcmp(temp_user.name, username) == 0) {
            // Move the file pointer to the position of the current record
            lseek(fd, curr_pos, SEEK_SET);

            // Update user details
            strncpy(temp_user.phone, phone, MAX_PHONE_LENGTH);
            strncpy(temp_user.password, password, MAX_PWD_LENGTH);

            // Write the updated user record to the file
            if (write(fd, &temp_user, sizeof(User)) == -1) {
                perror("Error updating user record");
                release_lock(fd);
                close(fd);
                exit(EXIT_FAILURE);
            }

            found = 1;
            break;
        }
        // Store the current position in case the user is found
        curr_pos = lseek(fd, 0, SEEK_CUR);
    }

    // Release lock after reading and writing
    release_lock(fd);
    close(fd);

    // Send status back to client
    if (found) {
        write(client_socket, "1", 1);
    } else {
        write(client_socket, "0", 1); // Send '0' to indicate user not found
    }
}



void handle_server_admin(int client_socket) {
    printf("Handling server admin\n");
    char choice[1];
    int exit = 0;
    while(!exit) {
        // Receive choice from server
        printf("Before read\n");
        read(client_socket, choice, 1);

        switch(choice[0] - '0') {
            case 1:
                add_book(client_socket);
                break;
            case 2:
                delete_book(client_socket);
                break;
            case 3:
                update_book(client_socket);
                break;
            case 4:
                search_book(client_socket);
                break;
            case 5:
                update_user(client_socket);
                break;
            case 6:
                exit = 1;
                break;
            default:
                printf("Invalid choice received from client.\n");
        }
    }
}

