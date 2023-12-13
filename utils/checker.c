#include "checker.h"
#include <ctype.h>
#include <string.h>

int digits_only(char *UID) {
    while (*UID) {
        if (isdigit(*UID++) == 0) return 0;
    }

    return 1;
}

int alphanumeric_only(char *pw) {
    while (*pw) {
        if (isalnum(*pw++) == 0) return 0;
    }
    return 1;
}

int alphanumeric_fname(char *fname) {
    while (*fname) {
        if (!(isalnum(*fname) || *fname == '-' || *fname == '_' || *fname == '.')) 
            return 0;
        fname++;
    }

    return 1;
}

int check_UID_password(char *UID, char *pw) {
    if (UID == NULL || strlen(UID) != 6 || !digits_only(UID) ||
        pw == NULL || strlen(pw) != 8 || !alphanumeric_only(pw))
        return 0;
    return 1;
}
 
int check_asset_name(char *name) {
    if (name == NULL || strlen(name) > 10 || !alphanumeric_fname(name))
        return 0;

    return 1;
}

int check_auction_start_value(char *start_value) {
    if (start_value == NULL || strlen(start_value) > 6 || !digits_only(start_value)) 
        return 0;

    return 1;
}

int check_auction_duration(char *duration) {
    if (duration == NULL || strlen(duration) > 5 || !digits_only(duration))
        return 0;

    return 1;
}

int check_file_name(char *fname) {
    if (fname == NULL || strlen(fname) > 24 || !alphanumeric_fname(fname))
        return 0;

    return 1;
}
