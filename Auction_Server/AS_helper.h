#ifndef AUCTION_SERVER_H
#define AUCTION_SERVER_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <ftw.h>

#define LIST_SIZE 4096

typedef struct {
   int AID[1000];
   int state[1000];
} AUCTIONLIST;

typedef struct {
   int no_bids;
   int bids[50];
} BIDLIST;


#define HOSTED 0
#define BIDDED 1
#define NOK 0
#define OK 1


// USER -----------------------------------------------------------------------------
int CreateUserDir(char *UID); 
int CheckUserDir(char *UID); 
int CheckUserRegistered(char *UID); 

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
int CreateAuctionFile(int mode, int AID, char *UID);
int StartAuction(int AID, char *UID, char *name, char *fname, int value, int t_active);
int EndAuction();
int CheckEndAuction();
int CheckAuctionExists(int AID);
int CheckOwnerAuction(int AID, char *UID);

// ASSET
int CreateAssetFile(int AID, char *fname, long fsize, char *fdata);
int CheckAssetFile(char *fname);

// LIST
int GetMyAuctionsList(int mode, char *UID, AUCTIONLIST *list);
int GetAuctionsList(AUCTIONLIST *list);
int ConvertAuctionList(int n, AUCTIONLIST *list, char *lt);

// BID
int LoadBid(char *pathname, BIDLIST *list);
int GetBidList(int AID, BIDLIST *list);

#endif
