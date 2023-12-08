#include "AS.h"

int fd,errcode;
int u,Verbose_mode = 0;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[256];
char request[20];
char reply[50];

// TODO: change the port so it it is 58000+[Group_number]
// INFO: The port 58001 only echoes the message received, the port 58011 is the actual AS_server
char *AS_port = "58011";


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

// LIN LOU UNR -------------------------------------------------
int login_request() {
    char UID[10];
    char pw[10];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);
    printf("%s %s\n", UID, pw);

    if (!check_syntax(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID)) {
        if (CheckPreviouslyRegistered(UID)) {
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
    printf("%s\n", reply);

    return 0;
}


int logout_request(){
    char UID[10];
    char pw[10];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);
    printf("%s %s\n", UID, pw);

    if (!check_syntax(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID) || !CheckPreviouslyRegistered(UID)) {
        if (CheckLogin(UID)) {
            EraseLogin(UID);
            strcpy(status,"OK");
        } else strcpy(status, "NOK");
    } else strcpy(status,"UNR");
       
    sprintf(reply,"RLO %s\n", status);
    printf("%s\n", reply);

    return 0;
}


int unresgister_request() {
    char UID[10];
    char pw[10];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);
    printf("%s %s\n", UID, pw);

    if (!check_syntax(UID,pw))
        strcpy(status,"ERR");
    else if (CheckUserDir(UID) || !CheckPreviouslyRegistered(UID)) {
        if (CheckLogin(UID) && CheckPassword(UID,pw)) {
            EraseLogin(UID);
            ErasePassword(UID);
            strcpy(status,"OK");
        } else strcpy(status, "NOK");
    } else strcpy(status,"UNR");
    
    sprintf(reply,"RUR %s\n", status);
    printf("%s\n", reply);

    return 0;
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
    }
    return 0;
}

// MAIN --------------------------------------------------------
int main(int argc, char **argv) {
    if (parse_args(argc, argv) == -1)
        exit(1);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        exit(1);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;

    printf("%c %s\n", Verbose_mode, AS_port);

    errcode = getaddrinfo(NULL, AS_port, &hints, &res);
    if (errcode != 0)
        exit(1);

    n = bind(fd,res->ai_addr, res->ai_addrlen);
    if(n == -1) /*error*/ exit(1);
  
    while (1) {
        addrlen = sizeof(addr);
        
        n = recvfrom(fd,buffer,256,0,(struct sockaddr*)&addr,&addrlen);
        if(n == -1)/*error*/exit(1);

        write(1,"received: ",10);write(1,buffer,n);

        u = process_user_request();
        if (u == -1) {
            write(1,"Error\n",6);
            continue;
        }
        
        n = sendto(fd,reply,strlen(reply),0,(struct sockaddr*)&addr,addrlen);
        if(n == -1)/*error*/exit(1);
    }

    freeaddrinfo(res);
    close(fd);

    return 0;
}
