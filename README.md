# IPK Project 1

## TCP/UDP Client in C

## Functionality

The program can establish a connection with a server over either TCP or UDP. Once connected, user can send messages to
the server and receive responses.

If connected via TCP, the connection can be terminated by sending the message "BYE" or if
there was an interrupt signal (<code>C-c</code>). After that, the program will wait for "BYE" message from the server
and then close.
If connected via UDP, the connection can only be terminated by the interrupt signal.

The program also includes a timeout function that will terminate the connection if the server does not respond within 30
seconds.

## Usage

To compile the program, run the following command:

    make

To run the program, run the following command:

    ./ipkcpc -h <server_ip> -p <server_port> -m <tcp/udp>

    -h <server_ip>      IPv4 address of the server
    -p <server_port>    Port number of the server
    -m <tcp/udp>        Connection mode (TCP or UDP)

Example:

    ./ipkcpc -h 127.0.0.1 -p 2023 -m tcp

## Testing

The program was tested on _merlin.fit.vutbr.cz_. <br>
Here are some outputs from the program:

Wrong input:

    xpasyn00@merlin: ~/ipk-project-1$ ./ipkcpc -h 127.0.0.1 -p 10002 -m qwerty
    Unknown mode: qwerty

    xpasyn00@merlin: ~/ipk-project-1$ ./ipkcpc  -h 147.229.176.19 -p abcd -m udp
    Invalid port number

    xpasyn00@merlin: ~/ipk-project-1$ ./ipkcpc  -h 147.229.176.256 -p 10002 -m udp
    Invalid host name


Connection via TCP:

    xpasyn00@merlin: ~/ipk-project-1$ ./ipkcpc -h 127.0.0.1 -p 10002 -m tcp
    HELLO
    HELLO
    SOLVE (* 10 4)
    RESULT 40
    SOLVE (- 2 8)
    RESULT -6
    BYE
    BYE

Connection with signal interrupt:
    
    xpasyn00@merlin: ~/ipk-project-1$ ./ipkcpc -h 127.0.0.1 -p 10002 -m udp
    (+ 42 42)
    OK: 84
    (a b -)
    ERR:  Invalid expression: b   a   - )
    
    (- 1 2)
    OK: -1
    ^C
    Closing socket and exiting...

    xpasyn00@merlin: ~/ipk-project-1$ ./ipkcpc -h 127.0.0.1 -p 10002 -m tcp
    HELLO
    HELLO
    SOLVE (+ 0 1)
    RESULT 1
    ^C
    Sent BYE message to server
    Received BYE from server
    
    Closing socket and exiting...


## Sources
- [Lectures](https://moodle.vut.cz/mod/folder/view.php?id=289124)
- [Project Git repository](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs)
- [Stack Overflow](https://stackoverflow.com/)
- [man](https://linux.die.net/man/)
