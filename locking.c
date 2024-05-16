#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void acquire_lock(int fd, int lock_type) {
    struct flock lock;
    lock.l_type = lock_type;  
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Failed to acquire lock");
        exit(EXIT_FAILURE);
    }
}

void release_lock(int fd) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Failed to release lock");
        exit(EXIT_FAILURE);
    }
}
