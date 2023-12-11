#ifndef AUCTION_SERVER_HELPER_H
#define AUCTION_SERVER_HELPER_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
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
#include <errno.h>

#define OK 0
#define NOK 1
#define REG 2
#define UNR 3
#define ERR 4

// USER -----------------------------------------------------------------------------
int CreateUserDir(char *UID); 
int CheckUserDir(char *UID); 
int CheckPreviouslyRegistered(char *UID); 

// PASSWORD
int CheckPassword(char *UID, char *pw); 
int CreatePassword(char *UID, char *pw); 
int ErasePassword(char *UID); 

// LOGIN
int CreateLogin(char *UID); 
int CheckLogin(char *UID); 
int EraseLogin(char *UID); 

// AUCTION ----------------------------------------------------------------------------
int CreateAUCTIONDir(int AID); 
int CheckAssetFile(char *fname); 

// AUXILIAR
int digits_only(char *UID); 
int alphanumeric_only(char *pw);
int check_syntax(char *UID, char *pw); 

#endif
