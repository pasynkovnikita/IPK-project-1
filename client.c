#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFSIZE 1024

void parse_args(int argc, char **argv, char **host, char **port, char **mode) {
    int i;
    *host = NULL;
    *port = NULL;
    *mode = NULL;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            if (i + 1 < argc) {
                *host = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Missing argument for option -h\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                *port = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Missing argument for option -p\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 < argc) {
                *mode = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Missing argument for option -m\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Unknown option %s\n", argv[i]);
            exit(1);
        }
    }
    if (*host == NULL || *port == NULL || *mode == NULL) {
        fprintf(stderr, "Missing one or more required options\n");
        exit(1);
    }
}

// function to handle tcp connection
void tcp(char *host, char *port) {
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR: socket");
        exit(1);
    }

    // Get address info
    struct hostent *server;
    struct sockaddr_in server_address;
    server = gethostbyname(host);

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(atoi(port));

    // Send request to server
    if (connect(sockfd, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }

    // Send request to server
    int bytestx, bytesrx, n;
    char buf[BUFSIZE];
    bzero(buf, BUFSIZE);

    while (strcmp(buf, "BYE\n") != 0) {
        // clear buffer
        bzero(buf, BUFSIZE);

        // Get next line of input
        fgets(buf, BUFSIZE, stdin);

        bytestx = send(sockfd, buf, strlen(buf), 0);
        if (bytestx < 0)
            perror("ERROR in sendto");

        // clear buffer
        bzero(buf, BUFSIZE);

        // Receive response from server
        bytesrx = recv(sockfd, buf, BUFSIZE, 0);
        if (bytesrx < 0)
            perror("ERROR in recvfrom");

        printf("Message from server: %s", buf);
    }

    // Close socket
    close(sockfd);
}


int main(int argc, char *argv[]) {
    char *host, *port, *mode;
    parse_args(argc, argv, &host, &port, &mode);
    if (strcmp(mode, "tcp") == 0) {
        tcp(host, port, mode);
    }
    return 0;
}