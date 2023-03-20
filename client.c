#ifdef __WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define BUFSIZE 1024

char BUF[BUFSIZE];  // Buffer for sending and receiving data
int SOCKFD = -1; // Socket is negative until it is created
char* STATE = "not connected"; // State for handling SIGINT over tcp connection

// Clears buffer
void clear_buffer() {
    bzero(BUF, BUFSIZE);
}

// Parses arguments
// @param argc number of arguments
// @param argv array of arguments
// @param host pointer to host
// @param port pointer to port
// @param mode pointer to mode
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
// @param host host name
// @param port port number
void tcp(char *host, char *port) {
    // get address info
    struct hostent *server;
    struct sockaddr_in server_address;
    server = gethostbyname(host);

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(atoi(port));

    // connect to server
    if (connect(SOCKFD, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }

    // set STATE to connected to handle SIGINT
    STATE = "connected";

    // number of bytes of sent and received data
    ssize_t bytestx, bytesrx;

    while (strcmp(BUF, "BYE\n") != 0) {
        clear_buffer();

        // get next line of input
        fgets(BUF, BUFSIZE, stdin);

        // send the message to server
        bytestx = send(SOCKFD, BUF, strlen(BUF), 0);
        if (bytestx < 0)
            perror("ERROR in send");

        clear_buffer();

        // receive response from server
        bytesrx = recv(SOCKFD, BUF, BUFSIZE, 0);
        if (bytesrx < 0)
            perror("ERROR in recv");

        // print response from server
        printf("%s", BUF);
    }

    // close socket
    close(SOCKFD);
}

// function to handle udp connection
// @param host host name
// @param port port number
void udp(char *host, char *port) {
    // get address info
    struct hostent *server;
    struct sockaddr_in server_address;
    server = gethostbyname(host);

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(atoi(port));

    // send request to server
    int bytestx, bytesrx, n;
    char sent_buf[BUFSIZE + 2];
    clear_buffer();

    while (strcmp(BUF, "BYE\n") != 0) {
        clear_buffer();

        // get next line of input
        fgets(BUF, BUFSIZE, stdin);
        // set opcode
        sent_buf[0] = 0;
        // set payload length
        sent_buf[1] = strlen(BUF);
        // set payload data
        memcpy(sent_buf + 2, BUF, strlen(BUF));

        bytestx = sendto(SOCKFD, sent_buf, strlen(BUF) + 2, 0, (const struct sockaddr *) &server_address,
                         sizeof(server_address));
        if (bytestx < 0)
            perror("ERROR in sendto");

        clear_buffer();

        // receive response from server
        bytesrx = recvfrom(SOCKFD, BUF, BUFSIZE, 0, NULL, NULL);
        if (bytesrx < 0)
            perror("ERROR in recvfrom");

        // print response from server
        int status_code = (int) BUF[1]; // buf[1] is status code
        printf("%s: %s\n", status_code == 0 ? "OK" : "ERR", BUF + 2); // buf + 2 to skip status code
    }
}


void sigint_handler() { //Handler for SIGINT
    //Reset handler to catch SIGINT next time.
    signal(SIGINT, sigint_handler);

    // if connected over tcp send BYE message and wait for BYE
    if (strcmp(STATE, "connected") == 0) {
        int bytestx = send(SOCKFD, "BYE\n", 4, 0);
        if (bytestx < 0)
            perror("ERROR in send");
        printf("\nSent BYE message to server\n");

        int bytesrx = recv(SOCKFD, BUF, BUFSIZE, 0);
        if (bytesrx < 0)
            perror("ERROR in recv");

        if (strcmp(BUF, "BYE\n") == 0)
            printf("Received BYE from server\n");
    }

    // clear buffer if not empty
    if (!strcmp(BUF, "")) {
        clear_buffer();
    }

    // close socket if it is open
    if (SOCKFD > 0) {
        printf("\nClosing socket and exiting...\n");
        close(SOCKFD);
    }


    fflush(stdout);

    exit(0);
}

int main(int argc, char *argv[]) {
    // signal handler for ctrl-c
    signal(SIGINT, sigint_handler);

    char *host, *port, *mode;
    parse_args(argc, argv, &host, &port, &mode);

    // create socket
    SOCKFD = socket(AF_INET, strcmp(mode, "tcp") == 0 ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (SOCKFD < 0) {
        perror("ERROR: socket");
        exit(1);
    }

    if (strcmp(mode, "tcp") == 0) {
        tcp(host, port);
    } else if (strcmp(mode, "udp") == 0) {
        udp(host, port);
    } else {
        fprintf(stderr, "Unknown mode %s", mode);
    }

    return 0;
}