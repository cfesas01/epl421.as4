#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tpool.h"

static const int port           = 3000;
static const size_t num_threads = 40;

void workerThread(void *params) {
    int workerSock = *(int *) params;
    char buffer[256];

    do {
        // Initialize buffer
        bzero(buffer, sizeof(buffer));

        // Get message
        if (read(workerSock, buffer, sizeof(buffer)) < 0) {
            perror("read");
            exit(1);
        }
        printf("Read string: %s\n", buffer);

        // Do something with input (buffer)
        strcpy(buffer, "That's the server response.\n");

        // Send message
        if (write(workerSock, buffer, sizeof(buffer)) < 0) {
            perror("write");
            exit(1);
        }
    } while (strcmp(buffer, "end") != 0);
    close(workerSock);
}

main(int argc, char *argv[]) {
    int sock, newSock, server_len, client_len;
    struct sockaddr_in server, client;
    struct sockaddr *server_ptr, *client_ptr;
    struct hostent *rem;
    tpool_t *thread_pool;
    size_t   i;

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
