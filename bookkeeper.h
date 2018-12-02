#ifndef BOOKKEEPER_H
#define BOOKKEEPER_H

#include "ytdl.h"

int bk_add(char name[], char url[], char dl_location[]);

int bk_list(int num_records, char *rtn[]);

int bk_info(char name[], struct ytdl_playlist *rtn);

int bk_rm(char name[]);

int bk_uncache(char name[]);

int bk_update(char name[]);

#endif
