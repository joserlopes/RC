#include "AS_helper.h"
#include "../utils/checker.h"

// USER -----------------------------------------------------------------------------
int CreateUserDir(char *UID) {
    char UID_dirname[13];
    char HOSTED_dirname[20];
    char BIDDED_dirname[20];
    int ret;

    sprintf(UID_dirname, "USERS/%s", UID);

    ret = mkdir(UID_dirname, 0700);
    if (ret == -1)
        return 0;
    
    sprintf(HOSTED_dirname, "USERS/%s/HOSTED", UID);

    ret = mkdir(HOSTED_dirname, 0700);
    if (ret == -1) {
        rmdir(UID_dirname);
        return 0;
    }

    sprintf(BIDDED_dirname, "USERS/%s/BIDDED", UID);

    ret = mkdir(BIDDED_dirname, 0700);
    if (ret == -1) {
        rmdir(UID_dirname);
        rmdir(HOSTED_dirname);
        return 0;
    }
    return 1;
}

int CheckUserDir(char *UID) {
    char user_name[35];
    DIR *dir ;

    sprintf(user_name, "USERS/%s", UID);

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
    char pass_path[34];
    FILE *fp;

    sprintf(pass_path, "USERS/%s/%s_pass.txt", UID, UID);

    if ((fp = fopen(pass_path, "r"))) {
        fclose(fp);
        return 1;
    }

    return 0;
}

// PASSWORD
int CheckPassword(char *UID, char *pw) {
    char pass_path[34];
    char password[9];
    FILE *fp;

    sprintf(pass_path, "USERS/%s/%s_pass.txt", UID, UID);

    if ((fp = fopen(pass_path, "r"))) {
        fgets(password, 9, fp);
        fclose(fp);
        if (!strcmp(password,pw))
            return 1;
    }
    return 0;
}

int CreatePassword(char *UID, char *pw) {
    char pass_path[34];
    FILE *fp;

    sprintf(pass_path, "USERS/%s/%s_pass.txt", UID, UID);

    fp = fopen (pass_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "%s\n", pw) ;
    fclose(fp);
    return 1;
}

int ErasePassword(char *UID) {
    char pass_path[34];
    
    sprintf(pass_path, "USERS/%s/%s_pass.txt",UID, UID);
    unlink(pass_path);
    return 1;
}

// LOGIN
int CreateLogin(char *UID) {
    char login_path[35];
    FILE *fp;

    sprintf(login_path, "USERS/%s/%s_login.txt", UID, UID);

    fp = fopen (login_path, "w");
    if (fp == NULL)
        return 0;

    fprintf(fp, "Logged in\n") ;
    fclose(fp);
    return 1;
}

int CheckLogin(char *UID) {
    char login_path[35];
    FILE *fp;

    sprintf(login_path, "USERS/%s/%s_login.txt", UID, UID);

    if ((fp = fopen(login_path, "r"))) {
        fclose(fp);
        return 1;
    }
    return 0;
}

int EraseLogin(char *UID) {
    char login_path[35];
    
    sprintf(login_path, "USERS/%s/%s_login.txt",UID, UID);
    unlink(login_path);
    return 0;
}

// AUCTION ----------------------------------------------------------------------------
int CreateAUCTIONDir(int AID) {
    char AID_dirname[15];
    char BIDS_dirname [20];
    int ret;

    if (AID < 1 || AID > 999)
        return 0;

    sprintf(AID_dirname, "AUCTIONS/%03d",AID);

    ret = mkdir(AID_dirname, 0700);
    if (ret == -1)
        return 0;

    sprintf(BIDS_dirname, "AUCTIONS/%03d/BIDS",AID);

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


// AUXILIAR
int digits_only(char *UID) {
    while (*UID) {
        if (isdigit(*UID++) == 0) return 0;
    }
    //int i;
    //while (UID[i] != '\0') {
	//	if (!(UID[i] >= '0' && UID[i] <= '9'))
	//		return 0;
	//	i++;
	//}
    return 1;
}

int alphanumeric_only(char *pw) {
    while (*pw) {
        if (isalnum(*pw++) == 0) return 0;
    }
    return 1;
}

int check_syntax(char *UID, char *pw) {
    if (UID == NULL || strlen(UID) != 6 || !digits_only(UID) ||
        pw == NULL || strlen(pw) != 8 || !alphanumeric_only(pw))
        return 0;
    return 1;
}

