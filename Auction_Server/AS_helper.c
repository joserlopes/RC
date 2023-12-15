#include "AS_helper.h"
#include "../utils/checker.h"

// USER -----------------------------------------------------------------------------
int CreateUserDir(char *UID) {
    char UID_dirname[28];
    char HOSTED_dirname[35];
    char BIDDED_dirname[35];
    int ret;

    sprintf(UID_dirname, "Auction_Server/USERS/%s", UID);

    ret = mkdir(UID_dirname, 0700);
    if (ret == -1)
        return 0;
    
    sprintf(HOSTED_dirname, "Auction_Server/USERS/%s/HOSTED", UID);

    ret = mkdir(HOSTED_dirname, 0700);
    if (ret == -1) {
        rmdir(UID_dirname);
        return 0;
    }

    sprintf(BIDDED_dirname, "Auction_Server/USERS/%s/BIDDED", UID);

    ret = mkdir(BIDDED_dirname, 0700);
    if (ret == -1) {
        rmdir(UID_dirname);
        rmdir(HOSTED_dirname);
        return 0;
    }
    return 1;
}

int CheckUserDir(char *UID) {
    char user_name[28];
    DIR *dir ;

    sprintf(user_name, "Auction_Server/USERS/%s", UID);

    if ((dir = opendir(user_name))) {
        /* Directory exists. */
        /* User is registered */
        closedir(dir);
    } else if (ENOENT == errno) {
        /* Directory does not exist. */
        /* User not registered */
        return 0;
    }
    return 1;
}

int CheckUserRegistered(char *UID) {
    char pass_path[44];
    FILE *fp;

    sprintf(pass_path, "Auction_Server/USERS/%s/%s_pass.txt", UID, UID);

    if ((fp = fopen(pass_path, "r"))) {
        fclose(fp);
        return 1;
    }

    return 0;
}

// PASSWORD
int CheckPassword(char *UID, char *pw) {
    char pass_path[44];
    char password[9];
    FILE *fp;

    sprintf(pass_path, "Auction_Server/USERS/%s/%s_pass.txt", UID, UID);

    if ((fp = fopen(pass_path, "r"))) {
        fgets(password, 9, fp);
        fclose(fp);
        if (!strcmp(password,pw))
            return 1;
    }
    return 0;
}

int CreatePassword(char *UID, char *pw) {
    char pass_path[44];
    FILE *fp;

    sprintf(pass_path, "Auction_Server/USERS/%s/%s_pass.txt", UID, UID);

    fp = fopen (pass_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "%s\n", pw) ;
    fclose(fp);
    return 1;
}

int ErasePassword(char *UID) {
    char pass_path[44];
    
    sprintf(pass_path, "Auction_Server/USERS/%s/%s_pass.txt",UID, UID);
    unlink(pass_path);
    return 1;
}

// LOGIN
int CreateLogin(char *UID) {
    char login_path[45];
    FILE *fp;

    sprintf(login_path, "Auction_Server/USERS/%s/%s_login.txt", UID, UID);

    fp = fopen (login_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "Logged in\n") ;
    fclose(fp);
    return 1;
}

int CheckLogin(char *UID) {
    char login_path[45];
    FILE *fp;

    sprintf(login_path, "Auction_Server/USERS/%s/%s_login.txt", UID, UID);

    if ((fp = fopen(login_path, "r"))) {
        fclose(fp);
        return 1;
    }
    return 0;
}

int EraseLogin(char *UID) {
    char login_path[45];
    
    sprintf(login_path, "Auction_Server/USERS/%s/%s_login.txt",UID, UID);
    unlink(login_path);
    return 0;
}

// AUCTION ----------------------------------------------------------------------------
int CreateAUCTIONDir(int AID) {
    char AID_dirname[28];
    char BIDS_dirname [33];
    int ret;

    if (AID < 1 || AID > 999)
        return 0;

    sprintf(AID_dirname, "Auction_Server/AUCTIONS/%03d",AID);

    ret = mkdir(AID_dirname, 0700);
    if (ret == -1)
        return 0;

    sprintf(BIDS_dirname, "Auction_Server/AUCTIONS/%03d/BIDS",AID);

    ret = mkdir(BIDS_dirname, 0700);
    if (ret == -1) {
        rmdir(AID_dirname);
        return 0;
    }
    return 1;
}

int CheckAssetFile(char *fname) {
    struct stat filestat;
    int retstat;
    retstat = stat(fname, &filestat);
    if (retstat == -1 || filestat.st_size == 0)
        return 0;
    return filestat.st_size;
}


int GetMyAuctionsList(int mode, char *UID, AUCTIONLIST *list) {
    struct dirent **filelist;
    AUCTIONLIST *l;
    FILE *fp;
    int i, n_entries, n_auctions, len;
    int AID;
    char dirname[36];
    char pathname[40];

    if (mode == HOSTED) 
        sprintf(dirname, "Auction_Server/USERS/%s/HOSTED/",UID);
    else sprintf(dirname, "Auction_Server/USERS/%s/BIDDED/",UID);
    
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could testfor -1 since n_entries count always with . and ..
        return 0;
    
    n_auctions = 0;
    for (i = 0; i < n_entries; i++) {
        len = strlen(filelist[i]->d_name);
        if (len == 7) { // Discard '.', '..' and invalid file names by size
            AID = atoi(strtok(filelist[i]->d_name,"."));
            sprintf(pathname,"Auction_Server/AUCTIONS/%03d/END_%03d.txt",AID,AID);
            if ((fp = fopen(pathname, "r"))) {
                fclose(fp);
                list->state[n_auctions] = 0;
            } else list->state[n_auctions] = 1;
            list->AID[n_auctions] = AID; 

            n_auctions++;
        }
        free(filelist[i]);
    }
    
    free(filelist);
    return n_auctions;
}

int GetAuctionsList(AUCTIONLIST *list) {
    struct dirent **filelist;
    AUCTIONLIST *l;
    FILE *fp;
    int i, n_entries, n_auctions, len;
    int AID;
    char dirname[25] = "Auction_Server/AUCTIONS/";
    char pathname[40];

    
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could testfor -1 since n_entries count always with . and ..
        return 0;
    
    
    n_auctions = 0;
    for (i = 0; i < n_entries; i++) {
        len = strlen(filelist[i]->d_name);
        if (len == 3) { // Discard '.', '..' and invalid file names by size
            AID = atoi(filelist[i]->d_name);
            sprintf(pathname,"Auction_Server/AUCTIONS/%03d/END_%03d.txt",AID,AID);
            if ((fp = fopen(pathname, "r"))) {
                fclose(fp);
                list->state[n_auctions] = 0;
            } else list->state[n_auctions] = 1;
            list->AID[n_auctions] = AID; 
            n_auctions++;
        }
        free(filelist[i]);
    }
    
    free(filelist);
    return n_auctions;
}

int ConvertAuctionList(int n, AUCTIONLIST *list, char *l) {
    int i;
    char aux[10];
    
    for (i=0; i<n; i++) {
        sprintf(aux, "%03d %d ",list->AID[i],list->state[i]);
        strcat(l,aux);
    }
    return 0;
}




int LoadBid(char *pathname, BIDLIST *list) {
    return 0;
}

int GetBidList(int AID, BIDLIST *list) {
    struct dirent **filelist;
    int n_entries, n_bids, len;
    char dirname[20];
    char pathname[32];
    char filename[11];

    sprintf(dirname, "AUCTIONS/%03d/BIDS/",AID);
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could testfor -1 since n_entries count always with . and ..
        return 0;
    n_bids = 0;
    list->no_bids = 0;
    while(n_entries--) {
        len = strlen(filelist[n_entries]->d_name);
        if (len == 10) { // Discard '.', '..' and invalid file names by size
            strcpy(filename,filelist[n_entries]->d_name);
            sprintf(pathname,"AUCTIONS/%03d/BIDS/%s",AID,filename);
            if (LoadBid(pathname,list))
                ++n_bids;
        }   
        free(filelist[n_entries]);
        if (n_bids == 50)
            break ;
    }
    free(filelist);
    return n_bids;
}
