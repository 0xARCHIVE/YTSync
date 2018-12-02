#ifndef YTDL_H
#define YTDL_H

struct ytdl_playlist;

int ytdl_init(void);

void ytdl_exit(void);

int ytdl_add(char * name, char * url, char * dl_location);

int ytdl_fetch_names(void);
int ytdl_fetch_next_name(int num_chars, char * rtn);

int ytdl_info(char * name, struct ytdl_playlist * rtn);

int ytdl_rename(char * name, char * new_name);

int ytdl_mv(char * name, char * dl_location);

int ytdl_rm(char * name);

int ytdl_uncache(char * name);

int ytdl_fetch(char * name);

void ytdl_init_struct(struct ytdl_playlist *rtn);

#endif
