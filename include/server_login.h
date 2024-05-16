#define LOGIN_SUCCESS 0
#define PASSWORD_MISMATCH 1
#define USER_NOT_FOUND 2

#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 20
#define MAX_PWD_LENGTH 30

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char password[MAX_PWD_LENGTH];
} User;


int create_user(char *name, char *phone, char *password);
int authenticate(char *name, char *password, char *filename);
