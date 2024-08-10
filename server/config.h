#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "hashtable.h"
#include "str_util.h"

#define IP "ip"
#define PORT "port"
#define THREAD_NUM "thread_num"
#define MYSQL_HOST "mysql_host"
#define MYSQL_USER "mysql_user"
#define MYSQL_PASSWD "mysql_passwd"
#define MYSQL_DB "mysql_db"

void readConfig(const char* filename, HashTable * ht);

#endif
