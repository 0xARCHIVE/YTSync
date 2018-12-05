#include "ytdl.h"
#include "input.h"
#include "bookkeeper.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define YTDL_MAX_NAME_LENGTH 256
#define YTDL_MAX_YTID_LENGTH 256
#define YTDL_MAX_URI_LENGTH 512
#define YTDL_LIST_NUM_RECORDS 10
#define YTDL_MAX_CMD_LENGTH 512

static int init = 0;
static int is_init(void);

// callbacks to register with input handler
static int ytdl_add_callback(int argc, char ** argv);
static int ytdl_list_callback(int argc, char ** argv);
static int ytdl_info_callback(int argc, char ** argv);
static int ytdl_rename_callback(int argc, char ** argv);
static int ytdl_mv_callback(int argc, char ** argv);
static int ytdl_rm_callback(int argc, char ** argv);
static int ytdl_uncache_callback(int argc, char ** argv);
static int ytdl_fetch_callback(int argc, char ** argv);

struct ytdl_playlist {
	char name[YTDL_MAX_NAME_LENGTH];
	char ytid[YTDL_MAX_YTID_LENGTH];
	char dl_location[YTDL_MAX_URI_LENGTH];
};

int ytdl_init(void) {
	if (is_init()) { return 1; }

	input_register_func("add",ytdl_add_callback);
	input_register_func("list",ytdl_list_callback);
	input_register_func("info",ytdl_info_callback);
	input_register_func("rename",ytdl_rename_callback);
	input_register_func("mv",ytdl_mv_callback);
	input_register_func("rm",ytdl_rm_callback);
	input_register_func("uncache",ytdl_uncache_callback);
	input_register_func("fetch",ytdl_fetch_callback);

	atexit(ytdl_exit);
	init = 1;

	return 1;
}

void ytdl_exit(void) {
	if (!is_init()){ return; }

	input_deregister_func("add");
	input_deregister_func("list");
	input_deregister_func("info");
	input_deregister_func("rename");
	input_deregister_func("mv");
	input_deregister_func("rm");
	input_deregister_func("uncache");
	input_deregister_func("fetch");

	init = 0;
}

static int is_init(void) {
	return init;
}

static int ytdl_add_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 3) { fprintf(stderr,"add expected 3 arguments (%d given)\n",argc); return 0; }

	if (ytdl_add(argv[0],argv[1],argv[2])) {
		fprintf(stdout,"Playlist added\n");
		return 1;
	}

	fprintf(stderr,"Failed to add playlist\n");
	return 0;
}

static int ytdl_list_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 0) { fprintf(stderr,"list expected 0 arguments (%d given)\n",argc);  return 0; }

	char ** buff = malloc(YTDL_LIST_NUM_RECORDS*sizeof(char *));
	for (int i=0; i<YTDL_LIST_NUM_RECORDS; i++) {
		buff[i] = malloc(YTDL_MAX_NAME_LENGTH*sizeof(char));
		buff[i][0] = '\0';
	}

	fprintf(stdout,"Playlists:\n");
	if (ytdl_fetch_names(YTDL_LIST_NUM_RECORDS,YTDL_MAX_NAME_LENGTH,&buff) != 0) {
		for (int i=0; i<YTDL_LIST_NUM_RECORDS; i++) {
			if (strlen(buff[i]) == 0) { continue; }
			fprintf(stdout,"\t%s\n",buff[i]);
		}
	}

	for (int i=0; i<YTDL_LIST_NUM_RECORDS; i++) {
		free(buff[i]);
	}

	free(buff);

	return 0;
}

static int ytdl_info_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 1) { fprintf(stderr,"info expected 1 argument (%d given)\n",argc);  return 0; }

	struct ytdl_playlist *playlist = (struct ytdl_playlist *)malloc(sizeof(struct ytdl_playlist));
	if (NULL == playlist) { return 0; }
	ytdl_init_struct(playlist);

	if (ytdl_info(argv[0],playlist) == 0) { return 0; }
	fprintf(stdout,"Playlist info:\n%s\t%s\t%s\n",playlist->name,playlist->dl_location,playlist->ytid);

	return 1;
}

static int ytdl_rename_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 2) { fprintf(stderr,"rename expected 2 arguments (%d given)\n",argc);  return 0; }

	if (ytdl_rename(argv[0],argv[1])) {
		fprintf(stdout,"Playlist renamed\n");
		return 1;
	}

	fprintf(stderr,"Failed to rename playlist\n");
	return 0;
}

static int ytdl_mv_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 2) { fprintf(stderr,"mv expected 2 arguments (%d given)\n",argc);  return 0; }

	if (ytdl_mv(argv[0],argv[1])) {
		fprintf(stdout,"Playlist moved\n");
		return 1;
	}

	fprintf(stderr,"Failed to move playlist\n");
	return 0;
}

static int ytdl_rm_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 1) { fprintf(stderr,"rm expected 1 argument (%d given)\n",argc);  return 0; }

	if (ytdl_rm(argv[0])) {
		fprintf(stdout,"Removed playlist\n");
		return 1;
	}

	fprintf(stderr,"Failed to remove playlist\n");
	return 0;
}

static int ytdl_uncache_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 1) { fprintf(stderr,"uncache expected 1 argument (%d given)\n",argc);  return 0; }

	if (ytdl_uncache(argv[0])) {
		fprintf(stdout,"Playlist uncached\n");
		return 1;
	}

	fprintf(stderr,"Failed to uncache playlist\n");
	return 0;
}

static int ytdl_fetch_callback(int argc, char ** argv) {
	if (!ytdl_init()) { return 0; }
	if (argc != 1) { fprintf(stderr,"fetch expected 1 argument (%d given)\n",argc);  return 0; }

	if (ytdl_fetch(argv[0])) {
		fprintf(stdout,"Playlist fetched\n");
		return 1;
	}

	fprintf(stderr,"Failed to fetch playlist\n");
	return 0;
}

int ytdl_add(char * name, char * ytid, char * dl_location) {
	if (!ytdl_init()) { return 0; }
	return bk_add(name,ytid,dl_location);
}

int ytdl_fetch_names(int num_records, int num_chars, char *** rtn) {
	if (!ytdl_init()) { return 0; }
	return bk_fetch_names(num_records,num_chars,rtn);
}

int ytdl_info(char * name, struct ytdl_playlist * rtn) {
	if (!ytdl_init()) { return 0; }
	return bk_info(name,rtn);
}

int ytdl_rename(char * name, char * new_name) {
	if (!ytdl_init()) { return 0; }
	return bk_rename(name,new_name);
}

int ytdl_mv(char * name, char * dl_location) {
	if (!ytdl_init()) { return 0; }
	return bk_mv(name,dl_location);
}

int ytdl_rm(char * name) {
	if (!ytdl_init()) { return 0; }
	return bk_rm(name);
}

int ytdl_uncache(char * name) {
	if (!ytdl_init()) { return 0; }
	return bk_uncache_playlist(name);
}

int ytdl_fetch(char * name) {
	if (!ytdl_init()) { return 0; }
	struct ytdl_playlist *playlist = (struct ytdl_playlist *)malloc(sizeof(struct ytdl_playlist));
	if (NULL == playlist) { return 0; }
	ytdl_init_struct(playlist);

	if (!bk_info(name,playlist)) { return 0; }

	// run list_playlist_videos.py to get list of video IDs in playlist
	FILE * fp;
	char * video_ytid;
	size_t len = 0;
	ssize_t nread;

	char command[YTDL_MAX_CMD_LENGTH];
	snprintf(command,YTDL_MAX_CMD_LENGTH,"%s \"%s\"","./list_playlist_videos.py",playlist->ytid);

	fp = popen(command,"r");
	if (fp == NULL) { return 0; }

	while ((nread = getline(&video_ytid, &len, fp)) != -1) {
		// remove trailing \n char
		char * pos;
		if ((pos = strchr(video_ytid,'\n')) != NULL) {
			*pos = '\0';
		}

		if (strncmp(video_ytid,"0",nread) == 0) {
			free(video_ytid);
			free(fp);
			return 0;
		}

		// ask bookkeeper if line has been cached
		if (bk_is_cached(name,video_ytid)) {
			fprintf(stdout,"Skipping video %s\n",video_ytid);
			continue;
		}

		// run fetch_video.py to download video ID
		FILE * fp2;
		char * line2;
		size_t len2 = 0;
		ssize_t nread2;

		char command2[YTDL_MAX_CMD_LENGTH];
		snprintf(command2,YTDL_MAX_CMD_LENGTH,"%s \"%s\" \"%s\"","./fetch_video.py",video_ytid,playlist->dl_location);

		fprintf(stdout,"Downloading video %s ... ",video_ytid);
		fflush(stdout);
		fp2 = popen(command2,"r");
		if (fp2 == NULL) { fprintf(stdout,"\n"); continue; }

		while ((nread2 = getline(&line2, &len2, fp2)) != -1) {
			// remove trailing \n char
			char * pos2;
			if ((pos2 = strchr(line2,'\n')) != NULL) {
				*pos2 = '\0';
			}

			if (strncmp(line2,"1",nread2) == 0) {
				fprintf(stdout,"done\n");
				bk_cache_video(name,video_ytid);
				continue;
			}

			fprintf(stdout,"\n");
			break;
		}

		free(line2);
		pclose(fp2);
	}

	free(video_ytid);
	pclose(fp);

	return 1;
}

void ytdl_init_struct(struct ytdl_playlist * rtn) {
	memset(rtn->name,'\0',sizeof(rtn->name));
	memset(rtn->ytid,'\0',sizeof(rtn->ytid));
	memset(rtn->dl_location,'\0',sizeof(rtn->dl_location));
}

void ytdl_set_name(struct ytdl_playlist * rtn, const char * name) {
	if (rtn == NULL) { return; }
	if (name == NULL) { return; }

	strncpy(rtn->name,name,YTDL_MAX_NAME_LENGTH);
}

void ytdl_set_ytid(struct ytdl_playlist * rtn, const char * ytid) {
	if (rtn == NULL) { return; }
	if (ytid == NULL) { return; }

	strncpy(rtn->ytid,ytid,YTDL_MAX_YTID_LENGTH);
}

void ytdl_set_dl_location(struct ytdl_playlist * rtn, const char * dl_location) {
	if (rtn == NULL) { return; }
	if (dl_location == NULL) { return; }

	strncpy(rtn->dl_location,dl_location,YTDL_MAX_URI_LENGTH);
}
