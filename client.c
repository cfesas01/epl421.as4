#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int port    = 3000;
static const char host[] = "localhost";

main(int argc, char *argv[]) {
    int sock, server_len;
    char buffer[256];
    struct sockaddr_in server;
    struct sockaddr *server_ptr;
    struct hostent *rem;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    rem = gethostbyname(host);
    if (rem == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    server.sin_family = PF_INET;
    bcopy((char *) rem->h_addr, (char *) &server.sin_addr, rem->h_length);
    server.sin_port = htons(port);
    server_ptr = (struct sockaddr *) &server;
    server_len = sizeof(server);

    if (connect(sock, server_ptr, server_len) < 0) {
        perror("connect");
        exit(1);
    }
    //printf("Requested connection to host %s, port %d\n", host, port);

    bzero(buffer, sizeof(buffer)); /* Initialize buffer */
    if (read(sock, buffer, sizeof(buffer)) < 0) { /* Receive message */
        perror("read");
        exit(1);
    }
    printf("> %s", buffer);
    int end = 0;
    do {
        bzero(buffer, sizeof(buffer)); /* Initialize buffer */
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin); /* Read message from stdin */
        buffer[strlen(buffer)-1] = '\0'; /* Remove newline character */

        if (write(sock, buffer, sizeof(buffer)) < 0) { /* Send message */
            perror("write");
            exit(1);
        }

        bzero(buffer, sizeof(buffer)); /* Initialize buffer */
        if (read(sock, buffer, sizeof(buffer)) < 0) { /* Receive message */
            perror("read");
            exit(1);
        }
        printf("> %s", buffer);
        if (strcmp(buffer, "+OK user POP3 server signing off") == 0)
            end = 1;

    } while(end == 0);

    printf("\n");
    close(sock);
    exit(0);
}