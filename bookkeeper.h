#ifndef BOOKKEEPER_H
#define BOOKKEEPER_H

#include "ytdl.h"

int bk_add(char * name, char * url, char * dl_location);

int bk_fetch_names(void);	// prepares bookkeeper to start fetching playlist names
int bk_fetch_next_name(int num_chars, char * rtn);	// returns 0 when we're at the end of the records

int bk_info(char * name, struct ytdl_playlist * rtn);

int bk_rename(char * name, char * new_name);

int bk_mv(char * name, char * dl_location);

int bk_rm(char * name);

int bk_cache_video(char * name, char * yt_id);

int bk_uncache_playlist(char * name);

int bk_is_cached(char * name, char * yt_id);

#endif
