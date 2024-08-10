//
// Created by 李勃鋆 on 24-8-7.
//

#ifndef DBPOOL_H
#define DBPOOL_H
#include "thread_pool.h"
#define POOL_SIZE 10  // 连接池大小

typedef struct {
	MYSQL **connections;
	int size;
	int current;
} DBPool;

DBPool *create_db_pool(const char *host, const char *user, const char *passwd, const char *db, int pool_size);

void destroy_db_pool(DBPool *pool);

MYSQL *get_db_connection(DBPool *pool);

void release_db_connection(DBPool *pool, MYSQL *conn);
#endif //DBPOOL_H

