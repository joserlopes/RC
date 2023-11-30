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
char buffer[256];
char *AS_addr = "tejo.tecnico.ulisboa.pt";
// TODO: change the port so it it is 58000+[Group_number]
// INFO: The port 58001 only echoes the message received, the port 58011 is the actual AS_server
char *AS_port = "58011";
char command_to_send[50];
char input[400];
char command[20];
char UID[7];
char password[9];
char name[11];
char asset_fname[30];
// Estes doubles precisam de ser doubles ou podem ser ints?
int start_value, time_active;
char AID[3];

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
    } else if (argc != 1) {
        return -1;
    }

    return 0;
}

int handle_login() {
    sscanf(input, "%*s %s %s", UID, password);
    sprintf(command_to_send, "LIN %s %s\n", UID, password);

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;

    return 0;
}

int handle_logout() {
    sprintf(command_to_send, "LOU %s %s\n", UID, password);

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;

    return 1;
}

int handle_unregister() {
    sprintf(command_to_send, "UNR %s %s\n", UID, password);

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;
    return 0;
}

int handle_exit() {
    return 0;
}

int handle_open() {
    sscanf(input, "%*s %s %s %d %d", name, asset_fname, &start_value, &time_active);

    return 0;
}

int handle_close() {
    sscanf(input, "%*s %s", AID);

    return 0;
}

int handle_myauctions() {
    sprintf(command_to_send, "LMA %s\n", UID);

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;

    return 0;
}

int handle_mybids() {
    sprintf(command_to_send, "LMB %s\n", UID);

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;

    return 0;
}

int handle_list() {
    sprintf(command_to_send, "LST\n");

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;

    return 0;
}

int handle_show_asset() {
    sscanf(input, "%*s %s", AID);

    return 0;
}

int handle_bid() {
    sscanf(input, "%*s %s", AID);

    return 0;
}

int handle_show_record() {
    sscanf(input, "%*s %s", AID);
    printf("%s\n", AID);
    sprintf(command_to_send, "SRC %s\n", AID);

    n = sendto(fd, command_to_send, strlen(command_to_send), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        return -1;

    return 0;
}

int receive_user_input() {
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;
    }


    sscanf(input, "%s", command);

    if (!strcmp(command, "login")) {
        handle_login();
    } else if (!strcmp(command, "logout")) {
        handle_logout();
    } else if (!strcmp(command, "unregister")) {
        handle_unregister();
    } else if (!strcmp(command, "exit")) {
        handle_exit();
    } else if (!strcmp(command, "open")) {
        handle_open();
    } else if (!strcmp(command, "close")) {
        handle_close();
    } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")) {
        handle_myauctions();
    } else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {
        handle_mybids();
    } else if (!strcmp(command, "list") || !strcmp(command, "l")) {
        handle_list();
    } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")) {
        handle_show_asset();
    } else if (!strcmp(command, "bid") || !strcmp(command, "b")) {
        handle_bid();
    } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")) {
        handle_show_record();
    } else {
        fprintf(stderr, "Error: [%s] command unknown\n", command);
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int r;
    if (parse_args(argc, argv) == -1)
        exit(1);
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        exit(1);
    
    memset(&hints, 0, sizeof(hints));
    memset(&buffer, 0, sizeof(buffer));
    memset(&command_to_send, 0, sizeof(command_to_send));
    memset(&input, 0, sizeof(input));
    memset(&command, 0, sizeof(command));
    memset(&UID, 0, sizeof(UID));
    memset(&password, 0, sizeof(password));
    memset(&name, 0, sizeof(name));
    memset(&asset_fname, 0, sizeof(asset_fname));
    
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    
    printf("%s %s\n", AS_addr, AS_port);
    
    errcode = getaddrinfo(AS_addr, AS_port, &hints, &res);
    if (errcode != 0) 
        exit(1);

    while (1) {
        printf("> ");
        r = receive_user_input();
        if (r == -1) {
            fprintf(stderr, "Error: error reading user input.\n");
            continue;
        }
        if (!strcmp(command, "exit")) {
            freeaddrinfo(res);
            close(fd);
            return 0;
        }

        addrlen = sizeof(addr);

        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1)
            exit(1);

        write(1, buffer, n);
    }
}
