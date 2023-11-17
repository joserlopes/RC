#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>

int fd,errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints,*res;
struct sockaddr_in addr;
char buffer[128];
char *AS_addr = "tejo.tecnico.ulisboa.pt";
// TODO: change the port so it it is 58000+[Group_number]
// INFO: The port 58001 only echoes the message received, the port 58011 is the actual AS_server
char *AS_port = "58001";

int parse_args(int argc, char **argv) {
    if (argc == 3) {
        // -n as the only flag
        if (strcmp(argv[1], "-n") == 0) {
            AS_addr = argv[2];
        }
        // -p as the only flag 
        if (strcmp(argv[1], "-p") == 0) {
            AS_port = argv[2];
        }
    } else if (argc == 5) {
        // -n as the first flag
        if (strcmp(argv[1], "-n") == 0) {
            AS_addr = argv[2];
            // -p as the second flag
            if (strcmp(argv[3], "-p") == 0) {
                AS_port = argv[4];
            }
        }
        // -p as the first flag
        if (strcmp(argv[1], "-p") == 0) {
            AS_port = argv[2];
            // -n as the second flag
            if (strcmp(argv[3], "-n") == 0) {
                AS_addr = argv[4];
            }
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    parse_args(argc, argv);
    printf("%s %s\n", AS_addr, AS_port);
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("%d\n", fd);
    if (fd == -1) /*error*/ exit(1);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;

    errcode = getaddrinfo(AS_addr, AS_port, &hints, &res);
    printf("%d\n", errcode);
    if (errcode != 0) /*error*/ exit(1);

    n = sendto(fd, "Hello!!!\n", 9, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /* error */ exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0,
                 (struct sockaddr*)&addr, &addrlen);
    if (n == -1) /* error */ exit(1);

    write(1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);

    return EXIT_SUCCESS;
}
