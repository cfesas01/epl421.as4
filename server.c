#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#include "tpool.h"

static const int port           = 3000;
static const size_t num_threads = 40;

char* substring(char *destination, const char *source, int beg, int n) {
    // extracts `n` characters from the source string starting from `beg` index
    // and copy them into the destination string
    while (n > 0)
    {
        *destination = *(source + beg);

        destination++;
        source++;
        n--;
    }

    // null terminate destination string
    *destination = '\0';

    // return the destination string
    return destination;
}
char *itoa(int num, char *str) {
    if(str == NULL)
        return NULL;

    sprintf(str, "%d", num);
    return str;
}

void copyFile(int num, char* name,int count,  char* currentdir) {
    char* s = (char*)malloc(1000 * sizeof(char));
    char* number = (char*)malloc(5 * sizeof(char));
    char* rename = (char*)malloc(5 * sizeof(char));
    strcpy(s, "cp as4-supplementary/");
    itoa(num, number);
    itoa(count, rename);
    strcat(s, number);
    strcat(s, " ");
    strcat(s, currentdir);
    strcat(s, "/mailbox/");
    strcat(s, name);
    strcat(s, "/");
    strcat(s, rename);
    system(s);
    free(s);
    free(number);
    free(rename);
}

int getCount(char* name) {
    int i;
    for (i = 1; i < 145; i++) {
        char* s = (char*)malloc(1000 * sizeof(char));
        strcpy(s, "mailbox/");
        strcat(s, name);
        strcat(s, "/");
        char* num = (char*)malloc(5 * sizeof(char));
        itoa(i, num);
        strcat(s, num);
        FILE* f = fopen(s, "rt");
        if (f == NULL) {
            return i;
        }
        fclose(f);
    }
}

void makeDirectory(char* name) {
    char* s = (char*)malloc(1000 * sizeof(char));
    strcpy(s, "mailbox/");
    strcat(s, name);
    DIR* dir = opendir(s);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
    }
    else { mkdir(s, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); }
}

void createMailbox() {
    mkdir("mailbox", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    char* currentdir = (char*)malloc(1000 * sizeof(char));
    getcwd(currentdir, 1000);
    printf("%s", currentdir);
    int i;
    for (i = 1; i < 145; i++) {
        char* file = (char*)malloc(5 * sizeof(char));
        itoa(i, file);
        char* s = (char*)malloc(1000 * sizeof(char));
        strcpy(s, "as4-supplementary/");
        strcat(s, file);
        FILE* f = fopen(s, "rt");
        if (f == NULL) {

        }
        else {
            int count = 0;
            char* line = (char*)malloc(1000 * sizeof(char));
            while (count < 3) {
                fgets(line, "%*[ \n]", f);
                count++;
            }
            while (fgetc(f) != ' ') {

            }


            char c = 'a';
            while (c != ':') {
                int check = 0;
                int i = 0;
                char  temp[1000];
                char c = fgetc(f);
                while (c != ' ' && c != '\n' && c != ',' && c != '\t') {
                    if (c == '@') {
                        check = 1;
                    }
                    temp[i] = c;
                    i++;
                    c = fgetc(f);
                    if (c == ':') break;
                }
                if (c == ':')break;
                if (check == 1) {
                    temp[i] = '\0';
                    makeDirectory(temp);
                    int count = getCount(temp);
                    copyFile(i, temp, count, currentdir);
                }

            }

            free(line);
            fclose(f);
        }
        free(s);
        free(file);

    }
    free(currentdir);
}

void workerThread(void *params) {
    int workerSock = *(int *) params;
    char buffer[256];
    char command[5];

    // Initialize buffer
    bzero(buffer, sizeof(buffer));
    // Send first message to client.
    strcpy(buffer, "+OK POP3 server ready\r\n");
    if (write(workerSock, buffer, sizeof(buffer)) < 0) {
        perror("write");
        exit(1);
    }

    int status = 1;
    int end = 0;
    do {
        // Initialize buffer
        bzero(buffer, sizeof(buffer));

        // Get message request from client.
        if (read(workerSock, buffer, sizeof(buffer)) < 0) {
            perror("read");
            exit(1);
        }
        printf("Input: %s\n", buffer);
        strncpy(command, buffer, 4);
        command[4] = 0; // null terminate destination
        int i = 0;
        for(i = 0; command[i]; i++){
            command[i] = tolower(command[i]);
        }

        // Do something with input (buffer)

        char* path = (char*)malloc(100 * sizeof(char));
        // Command QUIT
        if (strcmp(command, "quit") == 0) {
            strcpy(buffer, "+OK user POP3 server signing off");
            free(path);
            end = 1;
        }
        else if (strcmp(command, "user") == 0 && status == 1) {
            char username[sizeof(buffer)-5];

            substring(username, buffer, 5, sizeof(buffer));

            char* s = (char*)malloc(1000 * sizeof(char));
            strcpy(s, "mailbox/");
            strcat(s, username);
            strcat(s, "@enron.com");
            DIR* dir = opendir(s);
            if (dir) {
                sprintf(buffer, "+OK %s is a valid mailbox\r\n", username);
                strcpy(path, s);
                status = 2;
                closedir(dir);
            }
            else
                sprintf(buffer, "-ERR never heard of mailbox %s\r\n", username);

            free(s);
        }
        else if (strcmp(command, "pass") == 0 && status == 1) {
            strcpy(buffer, "do something pass\r\n");
        }
        else if (strcmp(command, "stat") == 0 && status == 2) {
            strcpy(buffer, "do something stat\r\n");
        }
        else if (strcmp(command, "retr") == 0 && status == 2) {
            strcpy(buffer, "do something retr\r\n");
        }
        else if (strcmp(command, "list") == 0 && status == 2) {
            strcpy(buffer, "do something list\r\n");
        }
        else if (strcmp(command, "dele") == 0 && status == 2) {
            strcpy(buffer, "do something dele\r\n");
        }
        else {
            strcpy(buffer, "-ERR unrecognized command\r\n");
        }
        // Send message
        if (write(workerSock, buffer, sizeof(buffer)) < 0) {
            perror("write");
            exit(1);
        }
    } while (end == 0);
    close(workerSock);
}

main(int argc, char *argv[]) {
    int sock, newSock, server_len, client_len;
    struct sockaddr_in server, client;
    struct sockaddr *server_ptr, *client_ptr;
    struct hostent *rem;
    tpool_t *thread_pool;
    size_t   i;

    //createMailbox();
    printf("Mailboxes created.\n\n");

    // Create thread pool
    thread_pool   = tpool_create(num_threads);

    // Create socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    server.sin_family = PF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    server_ptr = (struct sockaddr *) &server;
    server_len = sizeof(server);

    // Bind socket to address
    if (bind(sock, server_ptr, server_len) < 0) {
        perror("bind");
        exit(1);
    }

    // Listen for connections.
    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Listening for connections to port %d\n", port);

    while (1) {
        client_ptr = (struct sockaddr *) &client;
        client_len = sizeof(client);

        // Accept connection
        newSock = accept(sock, client_ptr, &client_len);
        if (newSock < 0) {
            perror("accept");
            exit(1);
        }

        rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family);
        if (rem == NULL) {
            herror("gethostbyaddr");
            exit(1);
        }
        printf("Accepted connection from %s\n", rem->h_name);

        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            perror("malloc");
            exit(1);
        }
        *arg = newSock;

        tpool_add_work(thread_pool, workerThread, arg);
    }
    close(sock);
    tpool_destroy(thread_pool);
}
