#include "AS_helper.h"
#include "../utils/checker.h"

time_t fulltime;
struct tm *current_time;

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
    char ASSET_dirname [34];
    int ret;

    if (AID < 1 || AID > 999)
        return 0;

    sprintf(AID_dirname, "Auction_Server/AUCTIONS/%03d", AID);

    ret = mkdir(AID_dirname, 0700);
    if (ret == -1)
        return 0;

    sprintf(BIDS_dirname, "Auction_Server/AUCTIONS/%03d/BIDS", AID);

    ret = mkdir(BIDS_dirname, 0700);
    if (ret == -1) {
        rmdir(AID_dirname);
        return 0;
    }

    sprintf(ASSET_dirname, "Auction_Server/AUCTIONS/%03d/ASSET", AID);

    ret = mkdir(ASSET_dirname, 0700);
    if (ret == -1) {
        rmdir(AID_dirname);
        rmdir(BIDS_dirname);
        return 0;
    }
    return 1;
}

int CreateAuctionFile(int mode, int AID, char *UID) {
    char auctionf_path[43];
    FILE *fp;

    if (mode == HOSTED)
        sprintf(auctionf_path,"Auction_Server/USERS/%s/HOSTED/%03d.txt", UID, AID);
    else sprintf(auctionf_path,"Auction_Server/USERS/%s/BIDDED/%03d.txt", UID, AID);

    fp = fopen (auctionf_path, "w");
    if (fp == NULL)
        return 0;

    if (mode == HOSTED)
        fprintf(fp, "HOSTING %03d\n", AID);
    else fprintf(fp, "BIDDING %03d\n", AID);
    fclose(fp);

}

int StartAuction(int AID, char *UID, char *name, char *fname, int value, int t_active) { 
    char start_path[42];
    char start_datetime[72];
    FILE *fp;
    time_t start_fulltime = time(&fulltime);
    current_time = gmtime(&fulltime);

    sprintf(start_datetime, "%4d-%02d-%02d %02d:%02d:%02d",
            current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/START_%03d.txt", AID, AID);

    fp = fopen (start_path, "w");
    if (fp == NULL)
        return 0;

    // UID name asset_fname start_value timeactive start_datetime start_fulltime
    fprintf(fp, "%s %s %s %d %d %s %ld\n",UID,name,fname,
            value,t_active,start_datetime,start_fulltime);
    fclose(fp);
    return 0;
}


long GetTimePassed(int AID) {
    int i=0;
    long fp_size;
    char *strtok_res;
    char *fbuffer;
    char start_path[42];
    FILE *fp;
    time_t time_passed;
    time_t start_fulltime;
    time_t end_fulltime = time(&fulltime);
    

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/START_%03d.txt", AID, AID);

    fp = fopen(start_path, "r");
    if (fp == NULL)
        return -1;

    fseek (fp , 0 , SEEK_END);
    fp_size = ftell (fp);
    rewind(fp);
    fbuffer = (char*) malloc (sizeof(char)*fp_size);
    memset(fbuffer,0,sizeof(fbuffer));

    fgets(fbuffer,fp_size,fp);
    fclose(fp);

    strtok_res = strtok(fbuffer, " ");
    
    while (i!=7) {strtok_res = strtok(NULL, " "); i++;}

    start_fulltime = strtol(strtok_res,NULL,10);

    time_passed = end_fulltime - start_fulltime;

    free(fbuffer);

    return time_passed;
}


int EndAuction(int AID) {
    char end_path[40];
    char end_datetime[72];
    FILE *fp;
    time_t end_sec_time;
    current_time = gmtime(&fulltime);

    sprintf(end_datetime, "%4d-%02d-%02d %02d:%02d:%02d",
            current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    sprintf(end_path, "Auction_Server/AUCTIONS/%03d/END_%03d.txt", AID, AID);

    end_sec_time = GetTimePassed(AID);

    fp = fopen (end_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "%s %ld\n", end_datetime, end_sec_time);
    fclose(fp);

    return 1;
}

int CheckAuctionEnd(int AID) {
    char end_path[40];
    FILE *fp;

    sprintf(end_path, "Auction_Server/AUCTIONS/%03d/END_%03d.txt", AID, AID);

    if ((fp = fopen(end_path, "r"))) {
        fclose(fp);
        return 1;
    }
    return 0;
}

int CheckAuctionTime(int AID) {
    int i=0;
    long fp_size;
    char *strtok_res;
    char *fbuffer;
    char start_path[42];
    char end_path[40];
    char end_datetime[72];
    FILE *fp;
    time_t end_sec_time;
    time_t time_active;
    time_t start_fulltime;
    time_t current_fulltime = time(&fulltime);
    struct tm *end_time;

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/START_%03d.txt", AID, AID);
    sprintf(end_path, "Auction_Server/AUCTIONS/%03d/END_%03d.txt", AID, AID);

    fp = fopen(start_path, "r");
    if (fp == NULL)
        return 0;

    fseek (fp , 0 , SEEK_END);
    fp_size = ftell (fp);
    rewind(fp);
    fbuffer = (char*) malloc (sizeof(char)*fp_size);
    memset(fbuffer,0,sizeof(fbuffer));

    fgets(fbuffer,fp_size,fp);
    fclose(fp);

    strtok_res = strtok(fbuffer, " ");
    while (i!=7) {
        strtok_res = strtok(NULL, " "); i++;
        if (i==4) time_active = strtol(strtok_res,NULL,10);
    }
    start_fulltime = strtol(strtok_res,NULL,10);

    printf("%03d time passed: %ld/%ld\n", AID, current_fulltime - start_fulltime, time_active);
    if(time_active > current_fulltime - start_fulltime)
        return 0;

    end_sec_time = time_active + start_fulltime;
    end_time = gmtime(&end_sec_time);
    sprintf(end_datetime, "%4d-%02d-%02d %02d:%02d:%02d",
            end_time->tm_year+1900, end_time->tm_mon+1, end_time->tm_mday,
            end_time->tm_hour, end_time->tm_min, end_time->tm_sec);

    fp = fopen (end_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "%s %ld\n", end_datetime, end_sec_time);
    fclose(fp);
    free(fbuffer);

    return 1;
}

int CheckAuctionExists(int AID) {
    char AID_dirname[28];
    DIR *dir;

    sprintf(AID_dirname, "Auction_Server/AUCTIONS/%03d", AID);

    if ((dir = opendir(AID_dirname))) {
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

int CheckAuctionOwner(int AID, char *UID) {
    char OWNER_UID[7];
    char start_path[42];
    FILE *fp;

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/START_%03d.txt", AID, AID);

    if ((fp = fopen(start_path, "r"))) {
        fgets(OWNER_UID,strlen(UID)+1,fp);
        if (!strcmp(OWNER_UID,UID)){
            fclose(fp);
            return 1;
        }    
        fclose(fp);
    }
    return 0;
}

int CheckAuctionsExpired() {
    char AID_dirname[28];
    char end_path[40];
    struct dirent **filelist;
    FILE *fp;
    int i, n_entries, n_auctions, n_expired, len;
    int AID;
    
    
    sprintf(AID_dirname, "Auction_Server/AUCTIONS/");

    n_entries = scandir(AID_dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could testfor -1 since n_entries count always with . and ..
        return 0;
    
    n_auctions = 0;
    n_expired = 0;
    for (i = 0; i < n_entries; i++) {
        len = strlen(filelist[i]->d_name);
        if (len == 3) { // Discard '.', '..' and invalid file names by size
            AID = atoi(filelist[i]->d_name);
            n_auctions++;
            if (!CheckAuctionEnd(AID)){ 
                if(CheckAuctionTime(AID))
                    n_expired++;
            }
        }
        free(filelist[i]);
    }
    
    free(filelist);
    return n_auctions;
}

int CreateAssetFile(int AID, char *fname, long fsize, char *fdata) {
    int len = strlen(fname);
    char asset_path[35+len];
    int file_creation;
    FILE *file;

    sprintf(asset_path, "Auction_Server/AUCTIONS/%03d/ASSET/%s", AID, fname);

    file = fopen(asset_path, "wb");
    file_creation = fwrite(fdata, 1, fsize, file);

    if (file_creation == -1) {
        return -1;
    }

    fclose(file);

    return 0;
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
            if (CheckAuctionEnd(AID)) {
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

int ConvertBidList(int n, BIDLIST *list, char *l) {
    int i;
    char aux[120];
    
    for (i=0; i<n; i++) {
        sprintf(aux, "B %03d %06d %s %ld ", list->UID[i], list->value[i],
                list->bid_datetime[i], list->bid_sec_time[i]);
        strcat(l,aux);
       
    }
    printf("%s\n", l);
    return 0;
}


int GetAuctionStart(int AID, char *auction_str) {
    long fp_size;
    FILE *fp;
    char *fbuffer;
    char start_path[42];

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/START_%03d.txt", AID, AID);

    fp = fopen(start_path, "r");
    if (fp == NULL)
        return -1;

    fseek (fp , 0 , SEEK_END);
    fp_size = ftell (fp);
    rewind(fp);
    fbuffer = (char*) malloc (sizeof(char)*fp_size);
    memset(fbuffer,0,sizeof(fbuffer));

    fgets(fbuffer,fp_size,fp);
    fclose(fp);
    
    strcpy(auction_str,fbuffer);

    printf("%s", fbuffer);

    free(fbuffer);

    return 0;
}


int GetAuctionEnd(int AID, char *end_str) {
    long fp_size;
    FILE *fp;
    char *fbuffer;
    char start_path[40];

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/END_%03d.txt", AID, AID);

    fp = fopen(start_path, "r");
    if (fp == NULL)
        return -1;

    fseek (fp , 0 , SEEK_END);
    fp_size = ftell (fp);
    rewind(fp);
    fbuffer = (char*) malloc (sizeof(char)*fp_size);
    memset(fbuffer,0,sizeof(fbuffer));

    fgets(fbuffer,fp_size,fp);
    fclose(fp);
    
    strcpy(end_str,fbuffer);

    printf("%s", fbuffer);

    free(fbuffer);

    return 0;
}


int LoadBid(char *pathname, BIDLIST *list) {
    int n = list->no_bids;
    long fp_size;
    char *fbuffer;
    char bid_path[42];
    char date[11];
    char time[9];
    FILE *fp;

    fp = fopen(pathname, "r");
    if (fp == NULL)
        return -1;

    fseek (fp , 0 , SEEK_END);
    fp_size = ftell (fp);
    rewind(fp);
    fbuffer = (char*) malloc (sizeof(char)*fp_size);
    memset(fbuffer,0,sizeof(fbuffer));

    fgets(fbuffer,fp_size,fp);
    fclose(fp);

    sscanf(fbuffer, "%d %d %s %s %ld", &list->UID[n], &list->value[n],
           date, time, &list->bid_sec_time[n]);
    strcpy(list->bid_datetime[n],date);
    strcat(list->bid_datetime[n]," ");
    strcat(list->bid_datetime[n],time);
    
    list->no_bids++;

    free(fbuffer);

    return 1;

}


int GetBidList(int AID, BIDLIST *list) {
    struct dirent **filelist;
    int n_entries, n_bids, len;
    char dirname[45];
    char pathname[45];
    char filename[11];

    sprintf(dirname, "Auction_Server/AUCTIONS/%03d/BIDS/",AID);
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could testfor -1 since n_entries count always with . and ..
        return 0;
    n_bids = 0;
    list->no_bids = 0;
    while(n_entries--) {
        len = strlen(filelist[n_entries]->d_name);
        if (len == 10) { // Discard '.', '..' and invalid file names by size
            strcpy(filename,filelist[n_entries]->d_name);
            sprintf(pathname,"Auction_Server/AUCTIONS/%03d/BIDS/%s",AID,filename);
            
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

int CreateBid(int AID, char *UID, int value) {
    char bid_path[45];
    char bid_datetime[72];
    FILE *fp;
    time_t bid_sec_time;
    current_time = gmtime(&fulltime);

    sprintf(bid_datetime, "%4d-%02d-%02d %02d:%02d:%02d",
            current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    sprintf(bid_path, "Auction_Server/AUCTIONS/%03d/BIDS/%06d.txt", AID, value);

    bid_sec_time = GetTimePassed(AID);

    fp = fopen (bid_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "%s %d %s %ld\n", UID, value, bid_datetime, bid_sec_time);
    fclose(fp);

}

int CheckStartValue (int AID, int value) {
    int i=0;
    long fp_size;
    char *strtok_res;
    char *fbuffer;
    char start_path[42];
    FILE *fp;
    int start_value;

    sprintf(start_path, "Auction_Server/AUCTIONS/%03d/START_%03d.txt", AID, AID);

    fp = fopen(start_path, "r");
    if (fp == NULL)
        return -1;

    fseek (fp , 0 , SEEK_END);
    fp_size = ftell (fp);
    rewind(fp);
    fbuffer = (char*) malloc (sizeof(char)*fp_size);
    memset(fbuffer,0,sizeof(fbuffer));

    fgets(fbuffer,fp_size,fp);
    fclose(fp);

    strtok_res = strtok(fbuffer, " ");
    
    while (i!=3) {strtok_res = strtok(NULL, " "); i++;}
    start_value = atoi(strtok_res);

    free(fbuffer);
    
    if (value > start_value)
        return 1;
    return 0;
}

int CheckAuctionBids(int AID, int value) {
    struct dirent **filelist;
    int n_entries, len, max_value, star_value;
    char dirname[45];
    char pathname[45];
    char filename[11];

    sprintf(dirname, "Auction_Server/AUCTIONS/%03d/BIDS/",AID);
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could testfor -1 since n_entries count always with . and ..
        return 0;

    
    while(n_entries--) {
        len = strlen(filelist[n_entries]->d_name);
        if (len == 10) { // Discard '.', '..' and invalid file names by size
            strcpy(filename,filelist[n_entries]->d_name);
            sprintf(pathname,"Auction_Server/AUCTIONS/%03d/BIDS/%s",AID,filename);
            max_value = atoi(strtok(filelist[n_entries]->d_name,"."));
            free(filelist);
            if (value == 0)
                return 1; 
            else if (value > max_value)
                return 1;
            return 0; 
        }   
        free(filelist[n_entries]);
    }
    free(filelist);

    if(value > 0)
        return CheckStartValue(AID,value);
    return 0;
}
