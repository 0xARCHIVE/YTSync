#ifndef YTDL_H
#define YTDL_H

struct ytdl_playlist;

int ytdl_init(void);

void ytdl_exit(void);

int ytdl_add(char * name, char * ytid, char * dl_location);

int ytdl_fetch_names(int num_records, int num_chars, char *** rtn);

int ytdl_info(char * name, struct ytdl_playlist * rtn);

int ytdl_rename(char * name, char * new_name);

int ytdl_mv(char * name, char * dl_location);

int ytdl_rm(char * name);

int ytdl_uncache(char * name);

int ytdl_fetch(char * name);

void ytdl_init_struct(struct ytdl_playlist * rtn);

void ytdl_set_name(struct ytdl_playlist * rtn, const char * name);

void ytdl_set_ytid(struct ytdl_playlist * rtn, const char * ytid);

void ytdl_set_dl_location(struct ytdl_playlist * rtn, const char * dl_location);

#endif
