#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>

int fd,errcode;
int u,Verbose_mode = 0;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[256];
char request[20];
char reply[50];

char *AS_addr = "tejo.tecnico.ulisboa.pt";
// TODO: change the port so it it is 58000+[Group_number]
// INFO: The port 58001 only echoes the message received, the port 58011 is the actual AS_server
char *AS_port = "58011";


int parse_args(int argc, char **argv) {
    switch (argc) {
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
    char UID[7];
    char pw[9];
    char status[4];
    sscanf(buffer, "%*s %s %s", UID, pw);
    printf("%s %s\n", UID, pw);

    if (UID == NULL || strlen(UID) != 6 || !digits_only(UID) ||
        pw == NULL || strlen(pw) != 8 || !alphanumeric_only(pw))
        strcpy(status,"ERR");

    sprintf(reply,"RLI %s", status);
    printf("%s\n", reply);

    return 0;
}

int digits_only(char* UID) {
    int i;
    while (*UID) {
        if (isdigit(*UID++) == 0) return 0;
    }
    //while (UID[i] != '\0') {
	//	if (!(UID[i] >= '0' && UID[i] <= '9'))
	//		return 0;
	//	i++;
	//}
    return 1;
}

int logout_request(){
    return 0;
}


// PROCESS REQUESTS --------------------------------------------
int process_user_request(){
    sscanf(buffer, "%s", request);

    if (!strcmp(request, "LIN")) {
        login_request();
    } else if (!strcmp(request, "LOU")) {
        logout_request();
    }
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
        
        u = process_user_request();
        if (u == -1) {
            write(1,"Error\n",6);
            continue;
        }

        write(1,"received: ",10);write(1,buffer,n);
        
        n = sendto(fd,buffer,n,0,(struct sockaddr*)&addr,addrlen);
        if(n == -1)/*error*/exit(1);
    }

    freeaddrinfo(res);
    close(fd);

    return EXIT_SUCCESS;
}
