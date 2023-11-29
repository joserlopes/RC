#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

int fd,errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
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
    } else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int handle_login(char *input, char *UID, char *password) {
    sscanf(input, "%*s %s %s", UID, password);

    return EXIT_SUCCESS;
}

int handle_logout() {
    puts("Hello!");

    return EXIT_SUCCESS;
}

int handle_unregister() {
    puts("Hello!");

    return EXIT_SUCCESS;
}

int handle_exit() {
    puts("Hello!");

    return EXIT_SUCCESS;
}

int handle_open(char *input, char *name, char *asset_fname, int *start_value, int *time_active) {
    sscanf(input, "%*s %s %s %d %d", name, asset_fname, start_value, time_active);

    return EXIT_SUCCESS;
}

int handle_close(char *input, int *AID) {
    sscanf(input, "%*s %d", AID);

    return EXIT_SUCCESS;
}

int handle_myauctions() {
    puts("Hello!");

    return EXIT_SUCCESS;
}

int handle_mybids() {
    puts("Hello!");

    return EXIT_SUCCESS;
}

int handle_list() {
    puts("Hello!");

    return EXIT_SUCCESS;
}

int handle_show_asset(char *input, int *AID) {
    sscanf(input, "%*s %d", AID);

    return EXIT_SUCCESS;
}

int handle_bid(char *input, int *AID) {
    sscanf(input, "%*s %d", AID);

    return EXIT_SUCCESS;
}

int handle_show_record(char *input, int *AID) {
    sscanf(input, "%*s %d", AID);

    return EXIT_SUCCESS;
}

int receive_user_input() {
    char input[400];
    char command[20];
    char UID[7];
    char password[9];
    char name[11];
    char asset_fname[30];
    // Estes doubles precisam de ser doubles ou podem ser ints?
    int start_value, time_active;
    int AID;

    fgets(input, sizeof(input), stdin);
    sscanf(input, "%s", command);

    if (!strcmp(command, "login")) {
        handle_login(input, UID, password);
    } else if (!strcmp(command, "logout")) {
        handle_logout();
    } else if (!strcmp(command, "unregister")) {
        handle_unregister();
    } else if (!strcmp(command, "exit")) {
        handle_exit();
    } else if (!strcmp(command, "open")) {
        handle_open(input, name, asset_fname, &start_value, &time_active);
    } else if (!strcmp(command, "close")) {
        handle_close(input, &AID);
    } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")) {
        handle_myauctions();
    } else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {
        handle_mybids();
    } else if (!strcmp(command, "list") || !strcmp(command, "l")) {
        handle_list();
    } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")) {
        handle_show_asset(input, &AID);
    } else if (!strcmp(command, "bid") || !strcmp(command, "b")) {
        handle_bid(input, &AID);
    } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")) {
        handle_show_record(input, &AID);
    } else {
        fprintf(stderr, "Error: [%s] command unknown\n", command);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // if (parse_args(argc, argv) == -1)
    //     exit(1);
    //
    // fd = socket(AF_INET, SOCK_DGRAM, 0);
    // if (fd == -1)
    //     exit(1);
    //
    // memset(&hints, 0, sizeof(hints));
    //
    // hints.ai_family=AF_INET;
    // hints.ai_socktype=SOCK_DGRAM;
    //
    // printf("%s %s\n", AS_addr, AS_port);
    //
    // errcode = getaddrinfo(AS_addr, AS_port, &hints, &res);
    // if (errcode != 0)
    //     exit(1);
    //
    // n = sendto(fd, "Hello!!!\n", 9, 0, res->ai_addr, res->ai_addrlen);
    // if (n == -1)
    //     exit(1);
    //
    // addrlen = sizeof(addr);
    //
    // n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
    // if (n == -1)
    //     exit(1);

    // write(1, "echo: ", 6);
    // write(1, buffer, n);
    //
    // freeaddrinfo(res);
    // close(fd);

    receive_user_input();

    return EXIT_SUCCESS;
}
