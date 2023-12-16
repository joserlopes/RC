#include "AS_helper.h"
#include "../utils/checker.h"

fd_set inputs, testfds;
int UDP_fd, TCP_fd, UDP_errcode, TCP_errcode;
int u,Verbose_mode = 0;
ssize_t UDP_n, TCP_n;
socklen_t UDP_addrlen, TCP_addrlen;
struct addrinfo UDP_hints, *UDP_res, TCP_hints, *TCP_res;
struct sockaddr_in UDP_addr, TCP_addr;
char in_str[50];
char buffer[LIST_SIZE];
char *accumulated_buffer = NULL;
char request[20];
char reply[LIST_SIZE];
int AUCTION = 1;

// TODO: change the port so it it is 58000+[Group_number]
// INFO: The port 58001 only echoes the message received, the port 58011 is the actual AS_server
char *AS_port = "58088";


int parse_args(int argc, char **argv) {
    switch (argc) {
    case 1:
        return 0;
    case 2:
        // -v as the only flag 
        if (strcmp(argv[1], "-v") == 0) 
            Verbose_mode = 1;
        break;
    case 3:
        // -p as the only flag
        if (strcmp(argv[1], "-p") == 0) 
            AS_port = argv[2];
        break;
    case 4: 
        // -p as the first flag
        if (strcmp(argv[1], "-p") == 0) {
            AS_port = argv[2];
            // -v as the second flag
            if (strcmp(argv[3], "-v") == 0) {
                Verbose_mode = 1;
            }
        }
        // -p as the first flag
        if (strcmp(argv[1], "-v") == 0) {
            Verbose_mode = 1;
            // -n as the second flag
            if (strcmp(argv[2], "-p") == 0) {
                AS_port = argv[3];
            }
        }
        break;
    default:
        return -1;
    }
    return 0;
}

void clean_exit_on_sig(int sig_num) {
    freeaddrinfo(UDP_res);
    freeaddrinfo(TCP_res);
    close(UDP_fd);
    close(TCP_fd);
    printf ("\n Signal %d received",sig_num);
}

// LIN LOU UNR -------------------------------------------------
int login_request() {
    char UID[10];
    char pw[10];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);

    if (!check_UID_password(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID)) {
        if (!CheckUserRegistered(UID)) {
            CreateLogin(UID);
            CreatePassword(UID,pw);
            strcpy(status,"REG");
        } else if (CheckPassword(UID,pw)) {
            CreateLogin(UID);
            strcpy(status,"OK");
        }
        else strcpy(status, "NOK");
    } else {
        CreateUserDir(UID);
        CreateLogin(UID);
        CreatePassword(UID,pw);
        strcpy(status,"REG");
    }
        
    sprintf(reply,"RLI %s\n", status);
    write(1,reply,strlen(reply));

    return 0;
}


int logout_request(){
    char UID[10];
    char pw[10];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);

    if (!check_UID_password(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID) && CheckUserRegistered(UID)) {
        if (CheckLogin(UID) && CheckPassword(UID,pw)) {
            EraseLogin(UID);
            strcpy(status,"OK");
        } else strcpy(status, "NOK");
    } else strcpy(status,"UNR");
       
    sprintf(reply,"RLO %s\n", status);
    write(1,reply,strlen(reply));

    return 0;
}


int unresgister_request() {
    char UID[10];
    char pw[10];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);

    if (!check_UID_password(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID) && CheckUserRegistered(UID)) {
        if (CheckLogin(UID) && CheckPassword(UID,pw)) {
            EraseLogin(UID);
            ErasePassword(UID);
            strcpy(status,"OK");
        } else strcpy(status, "NOK");
    } else strcpy(status,"UNR");
    
    sprintf(reply,"RUR %s\n", status);
    write(1,reply,strlen(reply));

    return 0;
}


// LIST USER AUCTIONS/BIDS
int listMyAuctions_request(int mode) {
    int n=0;
    char UID[10];
    char status[4];
    char *auction_list = (char*) malloc(sizeof(char)*4000);
    AUCTIONLIST *list = (AUCTIONLIST*) malloc(sizeof(AUCTIONLIST));

    memset(auction_list, 0, sizeof(auction_list));
    memset(list, 0, sizeof(list));

    sscanf(buffer, "%*s %s", UID);
    
    if (!check_UID(UID))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID) && CheckLogin(UID)) {
        n = GetMyAuctionsList(mode,UID,list);
    } else strcpy(status,"NLG");

    if (n>0) {
        strcpy(status,"OK");
        ConvertAuctionList(n,list,auction_list);
        write(1,auction_list,strlen(auction_list));write(1,"\n",1);
        if (mode == HOSTED) 
            sprintf(reply,"RMA %s %s\n", status, auction_list);
        else sprintf(reply,"RMB %s %s\n", status, auction_list);
    } else {
        strcpy(status,"NOK");
        if (mode == HOSTED) 
            sprintf(reply,"RMA %s\n", status);
        else sprintf(reply,"RMB %s\n", status);
    }
    write(1,reply,strlen(reply));

    free(list);
    free(auction_list);

    return 0;
}

// LIST ALL AUCTIONS
int listAuctions_request() {
    int n=0;
    char status[4];
    char *auction_list = (char*) malloc(sizeof(char)*4000);
    AUCTIONLIST *list = (AUCTIONLIST*) malloc(sizeof(AUCTIONLIST));
    
    memset(auction_list, 0, sizeof(auction_list));
    memset(list, 0, sizeof(list));

    n = GetAuctionsList(list);
    if (n == 0) strcpy(status,"NOK");
    else strcpy(status,"OK");

    ConvertAuctionList(n,list,auction_list);

    sprintf(reply,"RLS %s %s\n", status, auction_list);
    write(1,reply,strlen(reply));

    free(list);
    free(auction_list);

    return 0;
}

// OPEN NEW AUCTION
int openAuction_request() {
    char *UID;
    char *pw;
    int st = NOK;  
    char status[4];
    char *name;
    char *fname;
    int start_value;
    int time_active;
    ssize_t fsize;
    char *fdata;
    FILE *file;

    strtok(buffer, " ");
    UID = strtok(NULL, " ");
    pw = strtok(NULL, " ");
    name = strtok(NULL, " ");
    start_value = atoi(strtok(NULL, " "));
    time_active = atoi(strtok(NULL, " "));
    fname = strtok(NULL, " ");
    fsize = strtoul(strtok(NULL, " "), NULL, 10);
    fdata = strtok(NULL, "\0"); 

    if (!check_UID_password(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID) && CheckUserRegistered(UID)) {
        if (CheckLogin(UID) && CheckPassword(UID,pw) && 
            CreateAUCTIONDir(AUCTION)) {
            CreateAuctionFile(HOSTED,AUCTION,UID);
            StartAuction(AUCTION,UID,name,fname,start_value,time_active);
            CreateAssetFile(AUCTION,fname,fsize,fdata);
            strcpy(status,"OK");
            st = OK;
        } else strcpy(status, "NOK");
    }   else strcpy(status,"NLG");

    if (st == OK) {
        sprintf(reply,"ROA %s %03d\n", status, AUCTION);
        AUCTION++;
    } else sprintf(reply,"ROA %s\n", status);

    write(1,reply,strlen(reply));

    return 0;
}

// CLOSE AUCTION
int closeAuction_request() {
    char UID[10];
    char pw[10];
    int AID;
    char status[4];
    
    sscanf(buffer, "%*s %s %s %d", UID, pw, &AID);
    
    if (!check_UID_password(UID,pw))
        strcpy(status,"ERR");
    else if (!(CheckUserDir(UID) && CheckPassword(UID,pw))) 
        strcpy(status,"NOK");
    else if (!CheckLogin(UID))   
        strcpy(status,"NLG");
    else if (!CheckAuctionExists(AID))
        strcpy(status,"EAU");
    else if (!CheckOwnerAuction(AID,UID))
        strcpy(status,"EOW");
    else if (CheckEndAuction(AID))
        strcpy(status,"END");
    else {
        EndAuction(AID);
        strcpy(status,"OK");
    }
    
    sprintf(reply,"RUR %s\n", status);
    write(1,reply,strlen(reply));
}

// PROCESS REQUESTS --------------------------------------------
int process_user_request() {
    int r;

    sscanf(buffer, "%s", request);

    if (!strcmp(request, "LIN")) {
        login_request();
    } else if (!strcmp(request, "LOU")) {
        logout_request();
    } else if (!strcmp(request, "UNR")) {
        unresgister_request();
    } else if (!strcmp(request, "LMA")) {
        listMyAuctions_request(HOSTED);
    } else if (!strcmp(request, "LMB")) {
        listMyAuctions_request(BIDDED);
    } else if (!strcmp(request, "LST")) {
        listAuctions_request();
    } else if (!strcmp(request, "SRC")) {
        
    } else if (!strcmp(request, "OPA")) {
        openAuction_request();
    } else if (!strcmp(request, "CLS")) {
        closeAuction_request();
    }
    return 0;
}

// UDP --------------------------------------------------------------
void handle_UDP_connection() {
    UDP_addrlen = sizeof(UDP_addr);

    memset(buffer, 0, sizeof(buffer));
    memset(reply, 0, sizeof(reply));

    UDP_n = recvfrom(UDP_fd,buffer,sizeof(buffer),0,(struct sockaddr*)&UDP_addr,&UDP_addrlen);
    if(UDP_n == -1)/*error*/exit(1);

    write(1,"received: ",10);write(1,buffer,UDP_n);

    u = process_user_request();
    if (u == -1) {
        write(1,"Error\n",6);
    }
    
    UDP_n = sendto(UDP_fd,reply,strlen(reply),0,(struct sockaddr*)&UDP_addr,UDP_addrlen);
    if(UDP_n == -1)/*error*/exit(1);
}


// TCP ---------------------------------------------------------------
long read_TCP_loop(int new_TCP_fd, char *request_buffer, ssize_t size) {
    ssize_t received;
    ssize_t total_received = 0;
    memset(request_buffer,0,sizeof(buffer));

    while ((received = read(new_TCP_fd, request_buffer + total_received,
                    size - total_received)) > 0) {
        total_received += received;
        if(received<size)
            break;
    }
    if (received == -1) {
        return -1;
    }

    return total_received;
}

long write_TCP_loop(int new_TCP_fd,char *fdata_buffer, ssize_t size) {
    ssize_t written;
    ssize_t total_written = 0;
    
    while (total_written < size) {
        // fdata_buffer + total_written calculates the correct
        // offset in case multiple loops are necessary
        ssize_t written = write(new_TCP_fd, fdata_buffer + total_written, size);

        if (written == -1)
            return -1;

        total_written += written;
    }

    return 0;
}


void handle_TCP_connection() {
    TCP_addrlen = sizeof(TCP_addr);
    int new_TCP_fd = accept(TCP_fd, (struct sockaddr*)&TCP_addr, &TCP_addrlen);
    if (new_TCP_fd == -1) {
        printf("TCP closed\n");
        close(new_TCP_fd);
    }

    memset(buffer, 0, sizeof(buffer));
    memset(reply, 0, sizeof(reply));
    /*
    ssize_t accumulated_size = 0;
    
    do {
        TCP_n = read_TCP_loop(new_TCP_fd, buffer, sizeof(buffer));

        if (TCP_n > 0) {
            accumulated_buffer = (char *)realloc(accumulated_buffer, accumulated_size + TCP_n);
            if (accumulated_buffer == NULL) {
                free(accumulated_buffer);
                printf("buffer error\n");
                close(new_TCP_fd);
            }

            memcpy(accumulated_buffer + accumulated_size, buffer, TCP_n);
            accumulated_size += TCP_n;
        }

        memset(buffer, 0, sizeof(buffer));

    } while (TCP_n > 0);

    if (TCP_n == -1) {
        printf("error reading\n");
        close(new_TCP_fd);
    }
    */
    TCP_n = read_TCP_loop(new_TCP_fd, buffer, sizeof(buffer));
    if (TCP_n == -1) {
        printf("error reading\n");
        close(new_TCP_fd);
    }

    write(1,"received: ",10);write(1,buffer,TCP_n);

    u = process_user_request();
    if (u == -1) {
        write(1,"Error\n",6);
    }

    TCP_n = write_TCP_loop(new_TCP_fd, reply, strlen(reply));
    if (TCP_n == -1){
        printf("error writing\n");
        close(new_TCP_fd);
    }

    close(new_TCP_fd);
}


// MAIN --------------------------------------------------------
int main(int argc, char **argv) {
    signal(SIGSEGV, clean_exit_on_sig);
    if (parse_args(argc, argv) == -1)
        exit(1);
    printf("%c %s\n", Verbose_mode, AS_port);

    // UDP SERVER SECTION
    UDP_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_fd == -1)
        exit(1);

    memset(&UDP_hints, 0, sizeof(UDP_hints));
    UDP_hints.ai_family=AF_INET;
    UDP_hints.ai_socktype=SOCK_DGRAM;
    UDP_hints.ai_flags=AI_PASSIVE;

    UDP_errcode = getaddrinfo(NULL, AS_port, &UDP_hints, &UDP_res);
    if (UDP_errcode != 0)
        exit(1);

    UDP_n = bind(UDP_fd, UDP_res->ai_addr, UDP_res->ai_addrlen);
    if (UDP_n == -1) /*error*/ exit(1);
  
    // TCP SERVER SECTION
    TCP_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (TCP_fd == -1)
        exit(1);

    memset(&TCP_hints, 0, sizeof(TCP_hints));
    TCP_hints.ai_family=AF_INET;
    TCP_hints.ai_socktype=SOCK_STREAM;
    TCP_hints.ai_flags=AI_PASSIVE;

    TCP_errcode = getaddrinfo(NULL, AS_port, &TCP_hints, &TCP_res);
    if (TCP_errcode != 0)
        exit(1);

    TCP_n = bind(TCP_fd, TCP_res->ai_addr, TCP_res->ai_addrlen);
    if (TCP_n == -1) /*error*/ exit(1);

    if (listen(TCP_fd,5) == -1) exit(1);


    FD_ZERO(&inputs); // Clear input mask
    FD_SET(0,&inputs); // Set standard input channel on
    FD_SET(UDP_fd,&inputs); // Set UDP channel on
    FD_SET(TCP_fd,&inputs); // Set TCP channel on

    while (1) {
        testfds=inputs; // Reload mask
        // Use select to wait for activity on either socket
        if (select(FD_SETSIZE, &testfds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            break;
        }

        if(FD_ISSET(0,&testfds)) {
            fgets(in_str,49,stdin);
            if(!strcmp(in_str,"v\n"))
                switch (Verbose_mode) {
                case 0:
                    Verbose_mode = 1;
                    printf("---Verbose mode: ON\n");
                    break;
                case 1:
                    printf("---Verbose mode: OFF\n");
                    Verbose_mode = 0;
                    break;
                }
            else printf("---Input at keyboard: %s\n",in_str);
        }

        // Check if there is data on the UDP socket
        if (FD_ISSET(UDP_fd, &testfds)) {
            printf("udp\n");
            handle_UDP_connection();
        }

        // Check if there is a new TCP connection
        if (FD_ISSET(TCP_fd, &testfds)) {
            printf("tcp\n");
            handle_TCP_connection();
        }
    }

    freeaddrinfo(UDP_res);
    freeaddrinfo(TCP_res);
    close(UDP_fd);
    close(TCP_fd);

    return 0;
}
