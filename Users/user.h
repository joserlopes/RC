#ifndef USER_H
#define USER_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#define ERR_REPLY -2
#define LOGGED_IN -3
#define UNKNOWN_REPLY -4
#define BUFFER_SIZE 256
#define LIST_SIZE 5000

int parse_args(int argc, char **argv);
int receive_user_input();
// ---------------UDP-----------------
int handle_login();
int handle_logout();
int handle_unregister();
int handle_exit();
int handle_myauctions();
int handle_mybids();
int handle_list();
int handle_show_record();
// ---------------TCP-----------------
int initialize_TCP_connection();
long write_TCP_loop(char *fdata_buffer, ssize_t size);
long read_TCP_loop(char *reply_buffer, ssize_t size);
int handle_open();
int handle_close();
int handle_show_asset();
int handle_bid();

#endif
