#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/client.h"
#include <unistd.h>
#include "../include/server_login.h"

void print_menu(){
    printf("************************************\n");
    printf("1. Login as admin\n");
    printf("2. Login as user\n");
    printf("3. Create user account\n");
    printf("************************************\n\n");

    printf("Enter your choice: ");
}

int login(int sock){
    int choice, ret;
    char message[1];
    char buffer[1];
    char name[MAX_NAME_LENGTH];
    char password[MAX_PWD_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char message_to_send[MAX_NAME_LENGTH + MAX_PWD_LENGTH + 2];
    char message_create_user[MAX_NAME_LENGTH + MAX_PHONE_LENGTH + MAX_PWD_LENGTH + 3];

    while(1){
        print_menu();
        scanf("%d", &choice);
        getchar();

        if (choice >= 1 && choice <= 3){
            message[0] = '0' + choice;
            write(sock, message, 1);
 
            if (choice == 1 || choice == 2){
                printf("Enter name: ");
                fgets(name, MAX_NAME_LENGTH, stdin);
                name[strcspn(name, "\n")] = 0; // Remove trailing newline


                printf("Enter password: ");
                fgets(password, MAX_PWD_LENGTH, stdin);
                password[strcspn(password, "\n")] = 0; // Remove trailing newline
                
                sprintf(message_to_send, "%s:%s:", name, password);

                write(sock, message_to_send, strlen(message_to_send));
                read(sock, buffer, 1);

                ret = buffer[0] - '0';

                if(ret == LOGIN_SUCCESS){
                    printf("Authenticated succesfully!\n");
                    return choice;
                }

                else{
                    if(ret == PASSWORD_MISMATCH){
                        printf("Your password did not match!\n");

                    }
                    else if(ret == USER_NOT_FOUND){
                        printf("This user does not exist!\n");
                    }
                    continue;
                }

            }

            else if(choice == 3){
                printf("Enter name: ");
                fgets(name, MAX_NAME_LENGTH, stdin);
                name[strcspn(name, "\n")] = 0; // Remove trailing newline

                printf("Enter phone: ");
                fgets(phone, MAX_PHONE_LENGTH, stdin);
                phone[strcspn(phone, "\n")] = 0; // Remove trailing newline


                printf("Enter password: ");
                fgets(password, MAX_PWD_LENGTH, stdin);
                password[strcspn(password, "\n")] = 0; // Remove trailing newline

                sprintf(message_create_user, "%s:%s:%s:", name, phone, password);

                write(sock, message_create_user, strlen(message_create_user));
                read(sock, buffer, 1);
                
                // printf("buffer %s\n", buffer);
                ret = buffer[0] - '0';

                // printf("ret : %d\n", ret);
                if(ret == LOGIN_SUCCESS){
                    printf("Create user succesfully!\n");
                }
                else{
                    printf("Unable to create user\n");
                }

                
            }
            
        }

        else{
            printf("Invalid option. Try again.\n");
        }
    }
}
