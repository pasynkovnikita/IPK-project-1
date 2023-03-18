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
            }
            else {
                fprintf(stderr, "Missing argument for option -h\n");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                *port = argv[i + 1];
                i++;
            }
            else {
                fprintf(stderr, "Missing argument for option -p\n");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 < argc) {
                *mode = argv[i + 1];
                i++;
            }
            else {
                fprintf(stderr, "Missing argument for option -m\n");
                exit(1);
            }
        }
        else {
            fprintf(stderr, "Unknown option %s\n", argv[i]);
            exit(1);
        }
    }
    if (*host == NULL || *port == NULL || *mode == NULL) {
        fprintf(stderr, "Missing one or more required options\n");
        exit(1);
    }
}

int main (int argc, char * argv[]) {
    char *host, *port, *mode;
    parse_args(argc, argv, &host, &port, &mode);

    return 0;
}