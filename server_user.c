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
    printf("inside view books\n");
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

    printf("sending stuff\n");
    // Send the entire buffer to the client
    write(client_socket, buffer, 4096);

    // Free the allocated buffer
    free(buffer);
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
            case 3:
                exit = 1;
                break;
            default:
                printf("Invalid choice received from client.\n");
        }
    }
}

