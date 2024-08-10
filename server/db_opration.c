#include "db_opration.h"
#include "config.h"
#include "thread_pool.h"
#include <mysql/mysql.h>
#include <string.h>


MYSQL *connect_to_db(HashTable *ht) {
    // 1. 初始化mysql连接
    MYSQL *pconn = mysql_init(NULL);
    
    // 2. 获取sql连接的变量：host,user,passwd,db
    char *host = (char *)find(ht, MYSQL_HOST);
    char *user = (char *)find(ht, MYSQL_USER);
    char *passwd = (char *)find(ht, MYSQL_PASSWD);
    char *db = (char *)find(ht, MYSQL_DB);

    // 1.3 与数据库建立连接
    pconn = mysql_real_connect(pconn, host, user, passwd, db, 0, NULL, 0);
    if (pconn == NULL)
        error(1, mysql_errno(pconn), "mysql_real_connect");

    printf("mysql has connected.\n");

    return pconn;
}

void close_db(MYSQL *pconn) {
    mysql_close(pconn);
}

char *get_user_pwd(MYSQL *pconn, const char *username) {
    char *pwd = calloc(256, sizeof(char));
    // sql: SELECT path FROM user u JOIN virtualFile v 
    //      ON u.fileId = v.fileId WHERE userName = 'will'
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), 
        "SELECT path FROM user u JOIN virtualFile v "
        "ON u.fileId = v.fileId WHERE userName = '%s'", username);
    // printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return NULL;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return NULL;

    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL) 
        strcpy(pwd, row[0]);

    /* printf("pwd = %s\n", pwd); */
    
    mysql_free_result(pres);

    return pwd;
}

int get_user_id(MYSQL *pconn, const char *username) {
    int userId = -1;
    // sql: SELECT userId FROM user WHERE userName = 'will';
    char *query = "SELECT userId FROM user WHERE userName = ";
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "%s'%s'", query, username);

    // if (pconn == NULL) {
    //     fprintf(stderr, "Invalid MySQL connection handle.\n");
    //     return -1;
    // }


    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) {
        perror("mysql_store_result");
        return -1;
    }

    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL) 
        // printf("userId = %s\n", row[0]);
        userId = atoi(row[0]);
    
    mysql_free_result(pres);

    return userId;
}

/* ******************************************************************************* */
// file

int del_file_in_v(MYSQL *pconn, file_t file_type, int userId, const char * whole_path) {
    int hashId = -1;
    char *ftype = (file_type == 1) ? "Directory": "File";

    char sql[1024] = {0};
    // sql: SELECT hashId FROM virtualFile 
    //      WHERE type = 'File' AND userId = 2 AND path = '/will/1.txt';
    snprintf(sql, sizeof(sql), 
        "SELECT hashId FROM virtualFile WHERE "
        "type = '%s' AND userId = %d AND path = '%s'", 
        ftype, userId, whole_path);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return -1;

    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL) 
        // printf("userId = %s\n", row[0]);
        hashId = atoi(row[0]);
    
    mysql_free_result(pres);
    if (hashId == -1) {
        /* printf(RED "Notes:" RESET); */
        // TODO: send to client
        printf("no such file.\n");
        return -1;
    }

    // sql: DELETE FROM virtualFile 
    //      WHERE type = 'File' AND userId = 2 AND path = '/will/1.txt';
    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), 
        "DELETE FROM virtualFile WHERE "
        "type = '%s' AND userId = %d AND path = '%s'", 
        ftype, userId, whole_path);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 写入成功
    printf("del query ok.\n");

    return hashId;
}

int del_file_in_g(MYSQL *pconn, int hashId) {
    int lcount = -1;
    // UPDATE globalFile SET linkCount = linkCount - 1 WHERE hashId = 999;
    char sql[1024] = {0};
    
    snprintf(sql, sizeof(sql), 
        "UPDATE globalFile SET linkCount = linkCount - 1 WHERE "
        "hashId = %d", hashId);
    /* printf("sql = %s\n", sql); */

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 写入成功
    printf("update query ok.\n");

    // 需要返回linkCount
    memset(sql, 0, sizeof(sql));
    // SELECT linkCount FROM globalFile WHERE hashId = 999;
    snprintf(sql, sizeof(sql), 
        "SELECT linkCount FROM globalFile WHERE "
        "hashId = %d", hashId);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return -1;

    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL) 
        // printf("userId = %s\n", row[0]);
        lcount = atoi(row[0]);
    
    mysql_free_result(pres);

    return lcount;
}

int upd_file_in_g(MYSQL *pconn, int hashId) {
    // UPDATE globalFile SET lastLinkTime = NOW() WHERE hashId = 999;
    char sql[1024] = {0};
    
    snprintf(sql, sizeof(sql), 
        "UPDATE globalFile SET lastLinkTime = NOW() WHERE "
        "hashId = %d", hashId);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 写入成功
    printf("update time query ok.\n");
    
    return 0;
}

int get_hash(MYSQL *pconn, int hashId, char *filename[]) {
    char sql[1024] = {0};
    // sql: SELECT hash FROM globalFile WHERE hashId = 222;
    snprintf(sql, sizeof(sql),
        "SELECT hash FROM globalFile WHERE "
        "hashId = %d", hashId);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n",
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres)
        return -1;

    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL)
        // printf("userId = %s\n", row[0]);
        *filename = row[0];

    mysql_free_result(pres);
    if (hashId == -1) {
        // printf(RED "Notes:" RESET);
        printf("no such file.\n");
        return -1;
    }

    
    // update globalFile set isDel = 'true' WHERE hashId = 222;
    memset(sql, 0, sizeof(sql));
    
    snprintf(sql, sizeof(sql), 
        "UPDATE globalFile SET isDel = 'true' WHERE "
        "hashId = %d", hashId);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 写入成功
    printf("update isDel query ok.\n");

    return 0;
}

MYSQL_RES *get_last_time(MYSQL *pconn) {
    char sql[1024] = {0};
    // sql: SELECT hashId, lastLinkTime FROM globalFile WHERE lastLinkTime IS NOT NULL;
    snprintf(sql, sizeof(sql),
             "SELECT hashId, lastLinkTime FROM globalFile WHERE lastLinkTime IS NOT NULL");
    
    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return NULL;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return NULL;

    return pres;
}

/* ******************************************************************************* */
// dir

int del_dir_v(MYSQL *pconn, file_t file_type, int userId, const char * whole_path) {
    int dirId = -1;
    char *ftype = (file_type == 1) ? "Directory": "File";

    char sql[1024] = {0};

    // sql: SELECT fileId FROM virtualFile 
    //      WHERE type = 'Directory' AND userId = 2 AND path = '/file/will/aaa/eeee';
    snprintf(sql, sizeof(sql), 
        "SELECT fileId FROM virtualFile WHERE "
        "type = '%s' AND userId = %d AND path = '%s'", 
        ftype, userId, whole_path);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return -1;

    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL) 
        // printf("userId = %s\n", row[0]);
        dirId = atoi(row[0]);
    
    mysql_free_result(pres);
    if (dirId == -1) {
        /* printf(RED "Notes:" RESET); */
        // TODO:
        printf("no such dir.\n");
        return -1;
    }

    // DELETE FROM virtualFile 
    //      WHERE type = 'Directory' AND userId = 2 AND path = '/file/will/aaa/eeee';
    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), 
        "DELETE FROM virtualFile WHERE "
        "type = '%s' AND userId = %d AND path = '%s'", 
        ftype, userId, whole_path);
    /* printf("sql = %s\n", sql); */

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 写入成功
    printf("del query ok.\n");

    return dirId;
}

MYSQL_RES *get_dir_contents(MYSQL *pconn,int parentId) {
    char sql[1024] = {0};
    // sql: SELECT fileId, type, path FROM virtualFile WHERE parentId = 7;
    snprintf(sql, sizeof(sql), 
        "SELECT fileId, type, path FROM virtualFile WHERE "
        "parentId = %d", parentId);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return NULL;
    }
    printf("dir_contents, sql ok.\n");

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return NULL;
    printf("read dir contents ok.\n");

    return pres;
}

void free_result(MYSQL_RES *pres) {
    mysql_free_result(pres);
}


// 从 v表中获取dirID, 并更新u表fId
int upd_dir_in_u(MYSQL *pconn, int userId, int file_type, const char *whole_path) {

    char *ftype = (file_type == 1) ? "Directory": "File";
    
    int dirId = -1;

    /* SELECT fileId from virtualFile WHERE userId = 2 AND path = '/file/will/aaa/eeee'; */
    char sql[1024] = {0}; 
    snprintf(sql, sizeof(sql), 
        "SELECT fileId FROM virtualFile WHERE "
        "type = '%s' AND userId = %d AND path = '%s'", ftype, userId, whole_path);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 读取数据
    MYSQL_RES *pres = mysql_store_result(pconn);
    if (!pres) 
        return -1;

    // result empty 
    if (mysql_num_rows(pres) == 0) {
        printf("No rows found.\n");
        mysql_free_result(pres);
        return 0;  // 或者其他适当的值
    }


    // 读取结果集信息
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(pres)) != NULL) 
        // printf("userId = %s\n", row[0]);
        dirId = atoi(row[0]);
    
    mysql_free_result(pres);

    // UPDATE user SET fileId = 13 WHERE userId = 2;
    memset(sql, 0, sizeof(sql));
    
    snprintf(sql, sizeof(sql), 
        "UPDATE user SET fileId = %d WHERE "
        "userId = %d", dirId, userId);
    printf("sql = %s\n", sql);

    if (mysql_real_query(pconn, sql, strlen(sql))) {
        fprintf(stderr, "%d, %s\n", 
            mysql_errno(pconn), mysql_error(pconn));
        return -1;
    }

    // 写入成功
    printf("update user path query ok.\n");

    return 1;

}
