
#define MAX_BOOK_SIZE 100
#define MAX_AUTHOR_SIZE 100

#include "server_login.h"

typedef struct {
    int book_id;
    char name[MAX_BOOK_SIZE];
    char author[MAX_AUTHOR_SIZE];
    int deleted;
    int num_copies;
} Book;

typedef struct {
    char username[MAX_NAME_LENGTH];
    char bookname[MAX_BOOK_SIZE];
    int returned; // 0 means not returned, 1 means returned
} Borrow;

void handle_server_admin(int client_socket, char *name);