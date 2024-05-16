#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/server_login.h"
#include "../include/locking.h"
#include "../include/server.h"

int create_user(char *name, char *phone, char *password) {
    User user;
    int fd;

    // Open the file with write permission and create if not exists
    if ((fd = open(USERS_FILE, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Acquire lock before writing the new record
    acquire_lock(fd, F_WRLCK);

    // Find the last user ID by seeking to the end of the file
    off_t file_size = lseek(fd, 0, SEEK_END);
    user.id = file_size / sizeof(User) + 1; // Calculate next user ID based on file size

    // Populate the user structure
    strncpy(user.name, name, MAX_NAME_LENGTH - 1);
    user.name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
    strncpy(user.phone, phone, MAX_PHONE_LENGTH - 1);
    user.phone[MAX_PHONE_LENGTH - 1] = '\0'; // Ensure null-termination
    strncpy(user.password, password, MAX_PWD_LENGTH - 1);
    user.password[MAX_PWD_LENGTH - 1] = '\0'; // Ensure null-termination

    // Write user record to file
    if (write(fd, &user, sizeof(User)) == -1) {
        perror("Error writing to file");
        release_lock(fd); // Release lock before exiting
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Release lock after writing
    release_lock(fd);

    // Close the file
    close(fd);

    return LOGIN_SUCCESS;
}

int authenticate(char *name, char *password, char *filename) {
    User user;
    int fd;


    // Open the file with read permission
    if ((fd = open(filename, O_RDONLY)) == -1) {
        printf("%s\n", filename);
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Acquire read lock before reading
    acquire_lock(fd, F_RDLCK);

    // Search for the user by name
    int found = 0;
    while (read(fd, &user, sizeof(User)) > 0) {
        if (strcmp(user.name, name) == 0) {
            found = 1;
            break;
        }
    }

    // Release lock after reading
    release_lock(fd);
    close(fd);

    // printf("name :%s, found : %d\n", user.name, found);

    if (!found)
        return USER_NOT_FOUND;  // User not found

    // printf("user pwd: %s, password: %s\n", user.password, password);

    // Check if password matches
    if (strcmp(user.password, password) == 0)
        return LOGIN_SUCCESS;  // Authentication successful

    return PASSWORD_MISMATCH;  // Password mismatch
}

