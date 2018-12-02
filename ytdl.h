#ifndef YTDL_H
#define YTDL_H

struct ytdl_playlist;

int ytdl_add(char name[], char url[], char dl_location[]);

int ytdl_list(int num_records, char *rtn[]);

int ytdl_info(char name[], struct ytdl_playlist *rtn);

int ytdl_rm(char name[]);

int ytdl_uncache(char name[]);

int ytdl_fetch(char name[]);

struct ytdl_playlist* ytdl_allocate_struct(void);

#endif
