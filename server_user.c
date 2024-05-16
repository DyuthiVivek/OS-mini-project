#include <stdio.h>
#include <stdlib.h>
#include "locking.h"
#include "server_admin.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "server_user.h"
#include <unistd.h>
#include "server.h"
#include <string.h>


void view_books(int client_socket) {
    int fd;
    Book book;
    char *buffer;
    size_t buffer_size = 4096;
    size_t total_size = 0;

    // Open the book file for reading
    if ((fd = open("books.bin", O_RDONLY)) == -1) {
        perror("Error opening books file");
        exit(EXIT_FAILURE);
    }

    // Acquire shared lock before reading the file
    acquire_lock(fd, F_RDLCK);

    // Allocate initial buffer
    buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Error allocating memory");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Read each book record and append to the buffer
    while (read(fd, &book, sizeof(Book)) > 0) {
        if (!book.deleted) { // Skip deleted books
            char book_details[256];
            snprintf(book_details, sizeof(book_details), "ID: %d, Name: %s, Author: %s, Copies: %d\n", book.book_id, book.name, book.author, book.num_copies);
            
            size_t details_len = strlen(book_details);
            // Check if buffer needs to be resized
            if (total_size + details_len + 1 > buffer_size) {
                buffer_size *= 2;
                buffer = realloc(buffer, buffer_size);
                if (buffer == NULL) {
                    perror("Error reallocating memory");
                    release_lock(fd);
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }

            // Append book details to buffer
            strcpy(buffer + total_size, book_details);
            total_size += details_len;
        }
    }

    // Release the lock and close the file
    release_lock(fd);
    close(fd);

    // Send the entire buffer to the client
    write(client_socket, buffer, 4096);

    // Free the allocated buffer
    free(buffer);
}

void send_borrowed_books(int client_socket, const char *username) {
    int fd;
    Borrow borrow;
    char buffer[4096]; // Adjust size if necessary
    int buffer_len = 0;

    // Open the borrows file with read permission
    if ((fd = open("borrows.bin", O_RDONLY)) == -1) {
        perror("Error opening borrows file");
        exit(EXIT_FAILURE);
    }

    // Acquire read lock before reading
    acquire_lock(fd, F_RDLCK);

    // Read through the borrow records and collect book names borrowed by the specified user
    while (read(fd, &borrow, sizeof(Borrow)) > 0) {
        if (strcmp(borrow.username, username) == 0 && borrow.returned == 0) {
            int bookname_len = strlen(borrow.bookname);
            if (buffer_len + bookname_len + 2 > (int)sizeof(buffer)) {
                fprintf(stderr, "Buffer size exceeded while collecting book names.\n");
                break;
            }
            // Append the book name to the buffer
            snprintf(buffer + buffer_len, sizeof(buffer) - buffer_len, "%s\n", borrow.bookname);
            buffer_len += bookname_len + 1;
        }
    }

    // Release lock after reading
    release_lock(fd);
    close(fd);

    // Send the collected book names to the client
    if (buffer_len > 0) {
        buffer[buffer_len] = '\0';
        write(client_socket, buffer, 4096);
    }
    else{
        write(client_socket, "No books borrowed\n", 19);
    }
}

void handle_server_user(int client_socket, char *name) {
    char choice[1];
    int exit = 0;
    while(!exit) {
        // Receive choice from server
        read(client_socket, choice, 1);
        switch(choice[0] - '0') {
            case 1:
                printf("calling view books\n");
                view_books(client_socket);
                break;
            case 2:
                send_borrowed_books(client_socket, name);
                break;
            case 3:
                exit = 1;
                break;
            default:
                printf("Invalid choice received from client.\n");
        }
    }
}

