#include "bookkeeper.h"
#include "ytdl.h"

#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

#define BK_DB_NAME "db.db"

static int init = 0;
static int is_init(void);
static sqlite3 * db;

static int bk_init(void);
static void bk_exit(void);
static int bk_ensure_db(void);

static int bk_init(void) {
	if (is_init()) { return 1; }

	int rc = sqlite3_open(BK_DB_NAME,&db);
	if (rc != 0) {
		fprintf(stderr,"Can't open database %s\n",sqlite3_errmsg(db));
		return 0;
	}

	if (!bk_ensure_db()) { return 0; }

	atexit(bk_exit);
	init = 1;

	return 1;
}

static void bk_exit(void) {
	if (!is_init()) { return; }

	sqlite3_close(db);

	init = 0;
}

static int bk_ensure_db(void) {
	char * sql = 	"BEGIN TRANSACTION;" \
			"CREATE TABLE IF NOT EXISTS `video_cache` (" \
			"`ytid`	TEXT NOT NULL," \
			"`playlist_id`	INT NOT NULL," \
			"PRIMARY KEY(`ytid`,`playlist_id`)" \
			");" \
			"CREATE TABLE IF NOT EXISTS `playlists` (" \
			"`id` INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT," \
			"`name`	TEXT NOT NULL UNIQUE," \
			"`ytid`	TEXT NOT NULL," \
			"`dl_location`	TEXT NOT NULL" \
			");" \
			"COMMIT;";

	char * err = NULL;
	int rc = sqlite3_exec(db,sql,NULL,NULL,&err);

	if (rc != SQLITE_OK) {
		fprintf(stderr,"SQL error: %s\n",err);
		sqlite3_free(err);
		return 0;
	}

	return 1;
}

static int is_init(void) {
	return init;
}

int bk_add(char * name, char * ytid, char * dl_location) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"INSERT INTO `playlists` (name,ytid,dl_location) VALUES (?1,?2,?3);",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,name,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,2,ytid,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,3,dl_location,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr,"SQL error inserting: %s\n",sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

int bk_fetch_names(int num_records, int num_chars, char *** rtn) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"SELECT name FROM `playlists` LIMIT ?1;",-1,&stmt,NULL);
	sqlite3_bind_int(stmt,1,num_records);

	int rc;
	int i = 0;
	while (SQLITE_ROW == (rc = sqlite3_step(stmt))) {
		snprintf((*rtn)[i],num_chars,"%s",sqlite3_column_text(stmt,0));
		i++;
	}

	sqlite3_finalize(stmt);
	return 1;
}

int bk_info(char * name, struct ytdl_playlist * rtn) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"SELECT * FROM `playlists` WHERE name=?1 LIMIT 1;",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		ytdl_set_name(rtn,(char *)sqlite3_column_text(stmt,1));
		ytdl_set_ytid(rtn,(char *)sqlite3_column_text(stmt,2));
		ytdl_set_dl_location(rtn,(char *)sqlite3_column_text(stmt,3));

		sqlite3_finalize(stmt);
		return 1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

int bk_rename(char * name, char * new_name) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"UPDATE `playlists` SET name = ?1 WHERE name = ?2;",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,new_name,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,2,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr,"SQL error updating: %s\n",sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

int bk_mv(char * name, char * dl_location) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"UPDATE `playlists` SET dl_location = ?1 WHERE name = ?2;",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,dl_location,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,2,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr,"SQL error updating: %s\n",sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

int bk_rm(char * name) {
	if (!bk_init()) { return 0; }

	bk_uncache_playlist(name);

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"DELETE FROM `playlists` WHERE name = ?1;",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr,"SQL error updating: %s\n",sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

int bk_cache_video(char * name, char * ytid) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"INSERT INTO `video_cache` (ytid,playlist_id) SELECT ?1,id FROM `playlists` WHERE name=?2;",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,ytid,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,2,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr,"SQL error updating: %s\n",sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

int bk_uncache_playlist(char * name) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"DELETE FROM `video_cache` WHERE playlist_id=(SELECT id FROM `playlists` WHERE name=?1);",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr,"SQL error updating: %s\n",sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

int bk_is_cached(char * name, char * ytid) {
	if (!bk_init()) { return 0; }

	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db,"SELECT * FROM `video_cache` WHERE ytid=?1 AND playlist_id=(SELECT id FROM `playlists` WHERE name=?2) LIMIT 1;",-1,&stmt,NULL);
	sqlite3_bind_text(stmt,1,ytid,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,2,name,-1,SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (rc == SQLITE_ROW) { return 1; }

	return 0;
}
