#ifndef __DB_OP_H
#define __DB_OP_H

#include <stdlib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <mysql/client_plugin.h>
#include <string.h>

#include "hashtable.h"
#include "thread_pool.h"

#define MYSQL_HOST "mysql_host"
#define MYSQL_USER "mysql_user"
#define MYSQL_PASSWD "mysql_passwd"
#define MYSQL_DB "mysql_db"

void split_string(const char*, const char, char**, char**);
void read_config(const char *, HashTable *);

MYSQL *connect_to_db(HashTable *);
void close_db(MYSQL *);

char *get_user_pwd(MYSQL *, const char *);
int get_user_id(MYSQL *, const char *);

// file
int del_file_in_v(MYSQL *, file_t, int, const char *);
int del_file_in_g(MYSQL *, int);
int upd_file_in_g(MYSQL *, int);
int get_hash(MYSQL *pconn, int hashId, char *filename[]);

// dir
int del_dir_v(MYSQL *, file_t, int, const char *);
MYSQL_RES *get_dir_contents(MYSQL *,int);

void free_result(MYSQL_RES *pres);

int rm_command(MYSQL *pconn, const char *path, int userId);

int upd_dir_in_u(MYSQL *pconn, int userId, int, const char *whole_path);

#endif
