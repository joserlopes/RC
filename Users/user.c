#include "user.h"
#include "../utils/checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int UDP_fd, TCP_fd, UPD_errcode, TCP_errcode;
ssize_t UPD_n, TCP_n;
socklen_t UDP_addrlen, TCP_addrlen;
struct addrinfo UPD_hints, *UDP_res, TCP_hints, *TCP_res;
struct sockaddr_in UPD_addr, TCP_addr;
char server_reply[LIST_SIZE];
char *AS_addr = "localhost";
// TODO: change the default port so it it is 58000+[Group_number], in our case
// INFO: The port 58001 only echoes the message received, the port 58011 is
// the actual AS_server
char *AS_port = "58011";
char command_to_send[500];
char input[400];
char command[20];
char UID[7];
char password[9];
char name[11];
char asset_fname[30];
char AID[4];
char start_value[8];
char time_active[7];

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
    char status[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s %s", UID, password);
    sprintf(command_to_send, "LIN %s %s\n", UID, password);

    if (!check_UID_password(UID, password))
        return -1;

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
    UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UDP_addrlen = sizeof(UPD_addr);

    UPD_n = recvfrom(UDP_fd, server_reply, BUFFER_SIZE, 0,
            (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s", status);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "Successful login\n");
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "Incorrect login attempt\n");
        memset(UID, 0, sizeof(UID));
        memset(password, 0, sizeof(password));
    } else if (!strcmp(status, "REG")) {
        fprintf(stdout, "New user registered\n");
    } else if (!strcmp(status, "ERR")) {
        memset(UID, 0, sizeof(UID));
        memset(password, 0, sizeof(password));
        return ERR_REPLY;
    } else {
        memset(UID, 0, sizeof(UID));
        memset(password, 0, sizeof(password));
        return UNKNOWN_REPLY;
    }

    return 0;
}

int handle_logout() {
    char status[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s %s", UID, password);
    sprintf(command_to_send, "LOU %s %s\n", UID, password);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
            UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UDP_addrlen = sizeof(UPD_addr);

    UPD_n = recvfrom(UDP_fd, server_reply, BUFFER_SIZE, 0,
            (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s", status);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "Sucessful logout\n");
        memset(UID, 0, sizeof(UID));
        memset(password, 0, sizeof(password));
    } else if (!strcmp(status, "UNR")) {
        fprintf(stdout, "Unknown user\n");
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "User not logged in\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    return 0;
}

int handle_unregister() {
    char status[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s %s", UID, password);
    sprintf(command_to_send, "UNR %s %s\n", UID, password);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
            UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UDP_addrlen = sizeof(UPD_addr);

    UPD_n = recvfrom(UDP_fd, server_reply, BUFFER_SIZE, 0,
            (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s", status);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "Sucessful unregister\n");
    } else if (!strcmp(status, "UNR")) {
        fprintf(stdout, "Unknown user\n");
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "Incorrect unregister attempt\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    return 0;
}

int handle_exit() {
    // TODO: Ver a cena do User ter que informar que o utilizador ainda está
    // logado caso ele tente fazer exit
    return 0;
}

long write_TCP_loop(char *fdata_buffer, ssize_t size) {
    ssize_t written;
    ssize_t total_written = 0;

    while (total_written < size) {
        // fdata_buffer + total_written calculates the correct
        // offset in case multiple loops are necessary
        ssize_t written = write(TCP_fd, fdata_buffer + total_written, size);

        if (written == -1)
            return -1;

        total_written += written;
    }

    return 0;
}

long read_TCP_loop(char *reply_buffer, ssize_t size) {
    ssize_t received;
    ssize_t total_received = 0;

    while ((received = read(TCP_fd, reply_buffer + total_received,
                    size - total_received)) > 0) {
        total_received += received;
    }

    if (received == -1) {
        return -1;
    }

    return total_received;
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
    char asset_path[50];
    char status[10];
    char new_AID[10];
    int connection_status;

    sscanf(input, "%*s %s %s %s %s", name, asset_fname, start_value,
            time_active);

    if (!check_asset_name(name) || !check_auction_start_value(start_value) || !check_auction_duration(time_active))
        return -1;

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    FILE *file;

    sprintf(asset_path, "assets/%s", asset_fname);

    connection_status = initialize_TCP_connection();
    if (connection_status == -1)
        return -1;

    file = fopen(asset_path, "rb");

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

    sprintf(command_to_send, "OPA %s %s %s %d %d %s %ld %s\n", UID, password,
            name, atoi(start_value), atoi(time_active) * 60, asset_fname, size, fdata_buffer);

    TCP_n = write_TCP_loop(command_to_send, strlen(command_to_send));
    if (TCP_n == -1) {
        return -1;
    }

    TCP_n = read_TCP_loop(server_reply, sizeof(server_reply));
    if (TCP_n == -1) {
        return -1;
    }

    sscanf(server_reply, "%*s %s %s", status, new_AID);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "Auction %s created\n", new_AID);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "Auction could not be started\n");
    } else if (!strcmp(status, "NLG")) {
        fprintf(stdout, "User %s not logged in\n", UID);
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    freeaddrinfo(TCP_res);
    close(TCP_fd);

    free(fdata_buffer);
    fclose(file);

    return 0;
}

int handle_close() {
    char status[10];
    int connection_status;

    sscanf(input, "%*s %s", AID);

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    connection_status = initialize_TCP_connection();
    if (connection_status == -1)
        return -1;

    sprintf(command_to_send, "CLS %s %s %s\n", UID, password, AID);

    TCP_n = write_TCP_loop(command_to_send, strlen(command_to_send));
    if (TCP_n == -1) {
        return -1;
    }

    TCP_n = read_TCP_loop(server_reply, sizeof(server_reply));
    if (TCP_n == -1) {
        return -1;
    }

    sscanf(server_reply, "%*s %s", status);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "Auction %s successfully closed\n", AID);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "User %s doesn't exist or incorrect password\n", UID);
    } else if (!strcmp(status, "NLG")) {
        fprintf(stdout, "User %s not logged in\n", UID);
    } else if (!strcmp(status, "EAU")) {
        fprintf(stdout, "Auction %s doesn't exist\n", AID);
    } else if (!strcmp(status, "EOW")) {
        fprintf(stdout, "Auction %s not owned by user %s\n", AID, UID);
    } else if (!strcmp(status, "END")) {
        fprintf(stdout, "Auction %s has already finished\n", AID);
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    freeaddrinfo(TCP_res);
    close(TCP_fd);

    return 0;
}

int handle_myauctions() {
    char status[10];
    char auction_list[500];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LMA %s\n", UID);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
            UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, BUFFER_SIZE, 0,
            (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", status, auction_list);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "List of all auctions started by the logged in user:\n%s\n", auction_list);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "The logged in user hasn't started any auctions\n");
    } else if (!strcmp(status, "NLG")) {
        fprintf(stdout, "User not logged in\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    return 0;
}

int handle_mybids() {
    char status[10];
    char auction_list[500];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LMB %s\n", UID);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
            UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, BUFFER_SIZE, 0,
            (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", status, auction_list);

    if (!strcmp(status, "OK")) {
        fprintf(stdout,
                "List of all auctions in which the logged in user has placed "
                "bids:\n%s\n",
                auction_list);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "The logged in user hasn't placed any bids\n");
    } else if (!strcmp(status, "NLG")) {
        fprintf(stdout, "User not logged in\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    return 0;
}

int handle_list() {
    char status[10];
    char auction_list[LIST_SIZE];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sprintf(command_to_send, "LST\n");

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
            UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, LIST_SIZE, 0, (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", status, auction_list);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "List of all auctions:\n%s\n", auction_list);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "No auction started\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    return 0;
}

int CreateAssetFile(char *fname, char *fsize, char *fdata) {
    char fpath[27];
    FILE *fp;

    sprintf(fpath, "./%s", fname);
    fp = fopen(fpath, "w");
    if (fp == NULL)
        return -1;

    fprintf(fp, "%s", fdata);
    fclose(fp);

    return 0;
}

int handle_show_asset() {
    char *status;
    char *fname;
    char *fdata;
    char *fsizeStr;
    int connection_status;
    int file_creation;
    FILE *file;

    sscanf(input, "%*s %s", AID);

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));
    // memset(fdata, 0, sizeof(fdata));
    // memset(status, 0, sizeof(status));
    // memset(fname, 0, sizeof(fname));

    connection_status = initialize_TCP_connection();
    if (connection_status == -1)
        return -1;

    sprintf(command_to_send, "SAS %s\n", AID);

    TCP_n = write_TCP_loop(command_to_send, strlen(command_to_send));
    if (TCP_n == -1) {
        return -1;
    }

    char *accumulated_reply = NULL;

    ssize_t accumulated_size = 0;

    do {
        TCP_n = read_TCP_loop(server_reply, sizeof(server_reply));

        if (TCP_n > 0) {
            accumulated_reply = (char *)realloc(accumulated_reply, accumulated_size + TCP_n);
            if (accumulated_reply == NULL) {
                free(accumulated_reply);
                return -1;
            }

            memcpy(accumulated_reply + accumulated_size, server_reply, TCP_n);
            accumulated_size += TCP_n;
        }

        memset(server_reply, 0, sizeof(server_reply));

    } while (TCP_n > 0);

    if (TCP_n == -1) {
        return -1;
    }

    // sscanf(accumulated_reply, "%*s %s %s %ld %s", status, fname, &fsize, fdata);

    // Ignore the first part of the reply
    strtok(accumulated_reply, " ");
    status = strtok(NULL, " ");

    fname = strtok(NULL, " ");
    fsizeStr = strtok(NULL, " ");
    fdata = strtok(NULL, "\0"); 

    ssize_t fsize = strtoul(fsizeStr, NULL, 10);

    if (!strcmp(status, "OK")) {
        file = fopen(fname, "wb");
        file_creation = fwrite(fdata, 1, fsize, file);

        if (file_creation == -1) {
            return -1;
        }
        fprintf(stdout, "Stored file %s with size %ld\n", fname, fsize);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "No file to be sent\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    freeaddrinfo(TCP_res);
    close(TCP_fd);

    free(accumulated_reply);
    fclose(file);

    return 0;
}

int handle_bid() {
    int bid_value, connection_status;
    char status[10];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s %d", AID, &bid_value);

    connection_status = initialize_TCP_connection();
    if (connection_status == -1)
        return -1;

    sprintf(command_to_send, "BID %s %s %s %d\n", UID, password, AID, bid_value);

    TCP_n = write_TCP_loop(command_to_send, strlen(command_to_send));
    if (TCP_n == -1) {
        return -1;
    }

    TCP_n = read_TCP_loop(server_reply, sizeof(server_reply));
    if (TCP_n == -1) {
        return -1;
    }

    sscanf(server_reply, "%*s %s", status);

    if (!strcmp(status, "ACC")) {
        fprintf(stdout, "Successfully bidded %d on auction %s\n", bid_value, AID);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "Auction %s not active\n", AID);
    } else if (!strcmp(status, "NLG")) {
        fprintf(stdout, "User %s not logged in\n", UID);
    } else if (!strcmp(status, "REF")) {
        fprintf(stdout, "Bid value too low\n");
    } else if (!strcmp(status, "ILG")) {
        fprintf(stdout, "Cannot bid on an auction hosted by yourself\n");
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
    }

    freeaddrinfo(TCP_res);
    close(TCP_fd);

    return 0;
}

int handle_show_record() {
    char status[10];
    char auction_list[500];

    memset(command_to_send, 0, sizeof(command_to_send));
    memset(server_reply, 0, sizeof(server_reply));

    sscanf(input, "%*s %s", AID);

    sprintf(command_to_send, "SRC %s\n", AID);

    UPD_n = sendto(UDP_fd, command_to_send, strlen(command_to_send), 0,
            UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UPD_n == -1)
        return -1;

    UPD_n = recvfrom(UDP_fd, server_reply, BUFFER_SIZE, 0,
            (struct sockaddr *)&UPD_addr, &UDP_addrlen);
    if (UPD_n == -1)
        return -1;

    sscanf(server_reply, "%*s %s %[^\n]", status, auction_list);

    if (!strcmp(status, "OK")) {
        fprintf(stdout, "Information and status of auction %s\n%s\n", AID,
                auction_list);
    } else if (!strcmp(status, "NOK")) {
        fprintf(stdout, "Auction %s does't exist\n", AID);
    } else if (!strcmp(status, "ERR")) {
        return ERR_REPLY;
    } else {
        return UNKNOWN_REPLY;
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
        if (handler == UNKNOWN_REPLY) 
            fprintf(stderr, "Error logging in the user\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "logout")) {
        handler = handle_logout();
        if (handler == UNKNOWN_REPLY)
            fprintf(stdout, "Error logging out the user\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "unregister")) {
        handler = handle_unregister();
        if (handler == UNKNOWN_REPLY)
            fprintf(stdout, "Error unregistering user\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "exit")) {
        handle_exit();
    } else if (!strcmp(command, "open")) {
        handler = handle_open();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error opening auction\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "close")) {
        handler = handle_close();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error closing auction\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")) {
        handler = handle_myauctions();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error listing user's auctions\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {
        handler = handle_mybids();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error listing user's bids\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "list") || !strcmp(command, "l")) {
        handler = handle_list();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error listing the auctions present in the server\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")) {
        handler = handle_show_asset();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error showing asset\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "bid") || !strcmp(command, "b")) {
        handler = handle_bid();
        if (handler == UNKNOWN_REPLY) 
            fprintf(stderr, "Error placing bid\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")) {
        handler = handle_show_record();
        if (handler == UNKNOWN_REPLY)
            fprintf(stderr, "Error showing record\n");
        else if (handler == ERR_REPLY)
            return ERR_REPLY;
    } else {
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int r;

    if (parse_args(argc, argv) == -1) {
        fprintf(stderr, "Incorrect arguments. Exiting...\n");
        exit(1);
    }

    UDP_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_fd == -1) {
        fprintf(stderr, "Connection to the UPD socket failed. Exiting...\n");
        exit(1);
    }

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
    if (UPD_errcode != 0) {
        fprintf(stderr, "Error getting UPD address info failed. Exiting...\n");
        exit(1);
    }

    while (1) {
        printf("> ");
        r = receive_user_input();

        if (r == -1) {
            fprintf(stderr, "Error getting user input. Unknown command\n");
            continue;
        } else if (r == ERR_REPLY) {
            fprintf(stderr, "ERR reply received. Terminating interaction with the server...\n");
            return 0;
        }

        if (!strcmp(command, "exit")) {
            freeaddrinfo(UDP_res);
            close(UDP_fd);
            return 0;
        }

        memset(command, 0, sizeof(command));
    }
}
