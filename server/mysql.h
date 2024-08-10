//
// Created by 李勃鋆 on 24-8-5.
//

#ifndef MYSQL_H
#define MYSQL_H
#include "thread_pool.h"

MYSQL *initialize_db_connection(const char *host, const char *user, const char *passwd, const char *db);

void close_db_connection(MYSQL *conn);

void execute_query(MYSQL *conn, const char *sql, int page);

int execute_insert(MYSQL *conn, const char *sql);

int execute_update(MYSQL *conn, const char *sql);

int execute_delete(MYSQL *conn, const char *sql);
#endif //MYSQL_H

