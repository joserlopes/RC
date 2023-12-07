#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int UDP_fd, TCP_fd, UPD_errcode, TCP_errcode;
ssize_t UPD_n, TCP_n;
socklen_t UDP_addrlen, TCP_addrlen;
struct addrinfo UPD_hints, *UDP_res, TCP_hints, *TCP_res;
struct sockaddr_in UPD_addr, TCP_addr;
char server_reply[256];
char *AS_addr = "tejo.tecnico.ulisboa.pt";
// TODO: change the default port so it it is 58000+[Group_number], in our case 88
// INFO: The port 58001 only echoes the message received, the port 58011 is the actual AS_server
char *AS_port = "58011";
char command_to_send[500];
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
    switch (argc) {
        case 1:
            return 0;
        case 2:
            if (strcmp(argv[1], "-n") == 0) {
                AS_addr = argv[2];
            }
            // -p as the only flag 
            if (strcmp(argv[1], "-p") == 0) {
                AS_port = argv[2];
            }
            break;
        case 3:
            if (strcmp(argv[1], "-n") == 0) {
                AS_addr = argv[2];
            }
            // -p as the only flag 
            if (strcmp(argv[1], "-p") == 0) {
                AS_port = argv[2];
            }
            break;
        case 5:
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
            break;
        default:
            return -1;
    }

    return 0;
}

int handle_login() {
    char result[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s %s", UID, password);
    sprintf(command_to_send, "LIN %s %s\n", UID, password);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UDP_addrlen = sizeof(UPD_addr);

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s", result);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "Successful login\n");
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "Incorrect login attempt\n");
    } else if (!strcmp(result, "REG")) {
        fprintf(stdout, "New user registered\n");
    } else {
        fprintf(stdout, "Unknown server reply");
    }

    return 0;
}

int handle_logout() {
    char result[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LOU %s %s\n", UID, password);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UDP_addrlen = sizeof(UPD_addr);

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s", result);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "Sucessful logout\n");
    } else if (!strcmp(result, "UNR")) {
        fprintf(stdout, "Unknown user\n");
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "User not logged in\n");
    } else {
        fprintf(stdout, "Error logging out the user\n");
    }

    return 1;
}

int handle_unregister() {
    char result[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "UNR %s %s\n", UID, password);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UDP_addrlen = sizeof(UPD_addr);

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s", result);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "Sucessful unregister\n");
    } else if (!strcmp(result, "UNR")) {
        fprintf(stdout, "Unknown user\n");
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "Incorrect unregister attempt\n");
    } else {
        fprintf(stdout, "unknown server reply");
    }

    return 0;
}

int handle_exit() {
    return 0;
}

int initialize_TCP_connection() {
    TCP_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (TCP_fd == -1)
        return -1;

    memset(&TCP_hints, 0, sizeof(TCP_hints));
    TCP_hints.ai_family = AF_INET;
    TCP_hints.ai_socktype = SOCK_STREAM;

    TCP_errcode = getaddrinfo(AS_addr, AS_port, &TCP_hints, &TCP_res);
    if (TCP_errcode != 0)
        return -1;

    TCP_n = connect(TCP_fd, TCP_res->ai_addr, TCP_res->ai_addrlen);
    if (TCP_n == -1) {
        return -1;
    }

    return 0;
}

int handle_open() {
    char asset_name[50];
    char result[10];
    char new_AID[10];
    int connection_status;

    sscanf(input, "%*s %s %s %d %d", name, asset_fname, &start_value, &time_active);

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    FILE *file;

    sprintf(asset_name, "../assets/%s", asset_fname);

    connection_status = initialize_TCP_connection();
    if (connection_status == -1)
        return -1;

    file = fopen(asset_name, "rb");

    if (file == NULL) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fdata_buffer = (char *)malloc(sizeof(char) * size);
    if (fdata_buffer == NULL) {
        fclose(file);
        return -1;
    }

    memset(fdata_buffer, 0, strlen(fdata_buffer));

    size_t bytes_read = fread(fdata_buffer, 1, sizeof(char) * size, file);

    if (bytes_read != size) {
        free(fdata_buffer);
        fclose(file);
        return -1;
    }

    sprintf(command_to_send, "OPA %s %s %s %d %d %s %ld %s\n", UID, password, name, start_value, time_active * 60, asset_fname, size, fdata_buffer);

    TCP_n = write(TCP_fd, command_to_send, strlen(command_to_send));
    if (TCP_n == -1) 
        return -1;

    TCP_n = read(TCP_fd, server_reply, 256);
    if (TCP_n == -1) 
        return -1;

    sscanf(server_reply, "%*s %s %s", result, new_AID);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "Auction with AID: %s created\n", new_AID);
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "Auction could not be started\n");
    } else if (!strcmp(result, "NLG")) {
        fprintf(stdout, "User not logged in\n");
    } else {
        fprintf(stderr, "Error creating auction\n");
    }


    freeaddrinfo(TCP_res);
    close(TCP_fd);

    free(fdata_buffer);
    fclose(file);

    return 0;
}

int handle_close() {
    sscanf(input, "%*s %s", AID);

    return 0;
}

int handle_myauctions() {
    char result[10];
    char auction_list[500];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LMA %s\n", UID);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", result, auction_list);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "List of all auctions started by the logged in user:\n%s\n", auction_list);
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "The logged in user hasn't started any auctions\n");
    } else if (!strcmp(result, "NLG")) {
        fprintf(stdout, "User not logged in\n");
    } else {
        fprintf(stderr, "Error listing user auctions\n");
    }

    return 0;
}

int handle_mybids() {
    char result[10];
    char auction_list[500];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LMB %s\n", UID);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", result, auction_list);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "List of all auctions in which the logged in user has placed bids:\n%s\n", auction_list);
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "The logged in user hasn't placed any bids\n");
    } else if (!strcmp(result, "NLG")) {
        fprintf(stdout, "User not logged in\n");
    } else {
        fprintf(stderr, "Error listing user bids\n");
    }

    return 0;
}

int handle_list() {
    char result[10];
    char auction_list[1000];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LST\n");

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", result, auction_list);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "List of all auctions:\n%s\n", auction_list);
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "No auction started\n");
    } else {
        fprintf(stderr, "Error listing auctions\n");
    }
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
    char result[10];
    char auction_list[500];

    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s", AID);

    sprintf(command_to_send, "SRC %s\n", AID);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, 256, 0, (struct sockaddr*)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", result, auction_list);

    if (!strcmp(result, "OK")) {
        fprintf(stdout, "Information and status of auction: %s\n%s\n", AID, auction_list);
    } else if (!strcmp(result, "NOK")) {
        fprintf(stdout, "The auction with AID: %s does't exist\n", AID);
    } else {
        fprintf(stderr, "Error showing record\n");
    }

    return 0;
}

int receive_user_input() {
    int handler;
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;
    }

    sscanf(input, "%s", command);

    if (!strcmp(command, "login")) {
        handler = handle_login();
        if (handler == -1) 
            fprintf(stderr, "Error logging in the user\n");
    } else if (!strcmp(command, "logout")) {
        handle_logout();
    } else if (!strcmp(command, "unregister")) {
        handle_unregister();
    } else if (!strcmp(command, "exit")) {
        handle_exit();
    } else if (!strcmp(command, "open")) {
        handler = handle_open();
        if (handler == -1)
            fprintf(stderr, "Error opening asset\n");
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
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int r;

    if (parse_args(argc, argv) == -1)
        exit(1);
    
    UDP_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_fd == -1)
        exit(1);
    
    memset(&UPD_hints, 0, sizeof(UPD_hints));

    memset(server_reply, 0, sizeof(server_reply));
    memset(command_to_send, 0, sizeof(command_to_send));
    memset(input, 0, sizeof(input));
    memset(command, 0, sizeof(command));
    memset(UID, 0, sizeof(UID));
    memset(password, 0, sizeof(password));
    memset(name, 0, sizeof(name));
    memset(asset_fname, 0, sizeof(asset_fname));
    
    UPD_hints.ai_family = AF_INET;
    UPD_hints.ai_socktype = SOCK_DGRAM;
    
    printf("%s %s\n", AS_addr, AS_port);
    
    UPD_errcode = getaddrinfo(AS_addr, AS_port, &UPD_hints, &UDP_res);
    if (UPD_errcode != 0) 
        exit(1);

    while (1) {
        printf("> ");
        r = receive_user_input();

        if (r == -1) {
            fprintf(stderr, "Error getting user input\n");
            continue;
        }

        if (!strcmp(command, "exit")) {
            freeaddrinfo(UDP_res);
            close(UDP_fd);
            return 0;
        }
    }
}
