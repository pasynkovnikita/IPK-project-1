## Implemented functions

Main function:

    int main(int argc, char *argv[])

In the main function we create a socket, which will be a global variable. Then we parse the arguments from the command,
and according to the given mode, we call either tcp() or udp() function.

Parsing the arguments:

    void parse_args(int argc, char **argv, char **host, char **port, char **mode)

It takes the arguments from the command line and parses them into the variables that will be used in the program.

The key functions for the program are:

    tcp(char *host, char *port)
    udp(char *host, char *port)

They both work in a similar way. First, we initialize the variables needed for connection and connect to the server.
Then we start a loop, where we read the user input and send it to the server. After that, we wait for response from the
server and print it for the user.

Although if we are connected via UDP, we have to modify the message from the user according to UDP protocol.
First, we create a new buffer.

    char sent_buf[BUFSIZE + 2];

It has to be 2 bytes longer than the original message, because we have to insert opcode and payload length.

    sent_buf[0] = 0; // opcode 0 means that the message is a request
    sent_buf[1] = strlen(buf);  // payload length

Then we copy the original message into the new buffer.

    memcpy(sent_buf + 2, buf, strlen(buf));

After that, we can send the message to server and wait for response.

Function for handling the interrupt signal:

    void handle_sigint()

If the user wants to terminate connection with the server, he can use the interrupt signal (<code>C-c</code>).
This function will be called when the interrupt signal is received. First, the function will check if the program is
currently connected to server. If we are connected via TCP, we have to send server a message <code>"BYE"</code> and wait
for the <code>"BYE"</code> message from the server. Then we clean the buffer and close the socket. If we are connected
via UDP we don't have to send server anything so we just clean the buffer and close the socket.

Function for handling the timeout:

    void handle_timeout()

If the server does not respond within 30 seconds, the program will terminate the connection. Since we can't get a
response from the server, we don't have to send it anything. We just clean the buffer and close the socket.

## Limitations

To run the program on Windows I had to include `#ifdef` statements, because the program uses different libraries on
Windows and change the Makefile accordingly. I also had to change functions like `bzero()` and `bcopy()` to `memset()` and `memcpy()`. But when I tried to
test it on Windows, I ran into errors that I couldn't solve, so most probably the program won't work on Windows.

## Used libraries

The program uses the following libraries:

When run on Windows:

    winsock2.h
    ws2tcpip.h
    windows.h

When run on Linux:

    sys/socket.h
    netdb.h
    netinet/in.h

On all platforms:

    stdio.h
    stdlib.h
    string.h
    signal.h
    time.h
    unistd.h
