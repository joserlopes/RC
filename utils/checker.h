#ifndef CHECKER_H
#define CHECKER_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int digits_only(char *UID); 
int alphanumeric_only(char *pw);
int alphanumeric_fname(char *fname);
int check_UID(char *UID);
int check_UID_password(char *UID, char *pw); 
int check_asset_name(char *name);
int check_auction_start_value(char *start_value);
int check_auction_duration(char *duration);
int check_file_name(char *fname);

#endif 
