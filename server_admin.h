
#define MAX_BOOK_SIZE 100
#define MAX_AUTHOR_SIZE 100


typedef struct {
    int book_id;
    char name[MAX_BOOK_SIZE];
    char author[MAX_AUTHOR_SIZE];
    int deleted;
    int num_copies;
} Book;

void handle_server_admin(int client_socket);