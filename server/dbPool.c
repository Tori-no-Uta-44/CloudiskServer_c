//
// Created by 李勃鋆 on 24-8-7.
//
#include "thread_pool.h"
#include "dbPool.h"
#include "mysql.h"

DBPool *create_db_pool(const char *host, const char *user, const char *passwd, const char *db, int pool_size) {
	DBPool *pool = (DBPool *) malloc(sizeof(DBPool));
	if (pool == NULL) {
		fprintf(stderr, "Failed to allocate memory for DBPool\n");
		return NULL;
	}

	pool->connections = (MYSQL **) malloc(pool_size * sizeof(MYSQL *));
	if (pool->connections == NULL) {
		fprintf(stderr, "Failed to allocate memory for connections\n");
		free(pool);
		return NULL;
	}

	pool->size = pool_size;
	pool->current = 0;

	for (int i = 0; i < pool_size; ++i) {
		pool->connections[i] = initialize_db_connection(host, user, passwd, db);
		if (pool->connections[i] != NULL) {
			++pool->current;
		} else {
			fprintf(stderr, "Connection %d failed\n", i + 1);
		}
	}

	return pool;
}

void destroy_db_pool(DBPool *pool) {
	if (pool) {
		for (int i = 0; i < pool->size; ++i) {
			if (pool->connections[i]) {
				close_db_connection(pool->connections[i]);
			}
		}
		free(pool->connections);
		free(pool);
	}
}

MYSQL *get_db_connection(DBPool *pool) {//获取一个连接
	for (int i = 0; i < pool->size; ++i) {
		if (pool->connections[i]) {
			MYSQL *conn = pool->connections[i];
			pool->connections[i] = NULL;
			return conn;
		}
	}
	return NULL;
}

void release_db_connection(DBPool *pool, MYSQL *conn) {//释放一个连接
	for (int i = 0; i < pool->size; ++i) {
		if (pool->connections[i] == NULL) {
			pool->connections[i] = conn;
			return;
		}
	}
	mysql_close(conn);
}
