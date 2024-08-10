//
// Created by 李勃鋆 on 24-8-5.
//

#include "mysql.h"
MYSQL *conn = NULL;

MYSQL *initialize_db_connection(const char *host, const char *user, const char *passwd, const char *db) {
	MYSQL *conn = mysql_init(NULL);
	if (conn == NULL) {
		printf("mysql_init() failed\n");
		return NULL;
	}

	if (mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0) == NULL) {
		printf("mysql_real_connect() failed\n");
		mysql_close(conn);
		return NULL;
	}

	return conn;
}

void close_db_connection(MYSQL *conn) {
	mysql_close(conn);
}

int execute_insert(MYSQL *conn, const char *sql) {
	if (mysql_real_query(conn, sql, strlen(sql))) {
		printf("添加失败: %d, %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	return mysql_affected_rows(conn);
}

void execute_query(MYSQL *conn, const char *sql, int page) {
	int limit = 32;
	int offset = (page - 1) * limit;
	char paginated_sql[256];
	snprintf(paginated_sql, sizeof(paginated_sql), "%s LIMIT %d, %d", sql, offset, limit);

	if (mysql_real_query(conn, paginated_sql, strlen(paginated_sql))) {
		printf("查询失败: %d, %s\n", mysql_errno(conn), mysql_error(conn));
		return;
	}

	MYSQL_RES *result = mysql_store_result(conn);
	if (result) {
		int num_fields = mysql_num_fields(result);
		MYSQL_ROW row;

		// 列
		MYSQL_FIELD *field;
		while ((field = mysql_fetch_field(result))) {
			printf("%s\t", field->name);
		}
		printf("\n");

		// 行
		while ((row = mysql_fetch_row(result))) {
			for (int i = 0; i < num_fields; i++) {
				printf("%s\t", row[i] ? row[i] : "NULL");
			}
			printf("\n");
		}

		mysql_free_result(result);
	}
}

int execute_delete(MYSQL *conn, const char *sql) {
	if (mysql_real_query(conn, sql, strlen(sql))) {
		printf("删除失败: %d, %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	return mysql_affected_rows(conn);
}

int execute_update(MYSQL *conn, const char *sql) {
	if (mysql_real_query(conn, sql, strlen(sql))) {
		printf("更新失败: %d, %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	return mysql_affected_rows(conn);
}

