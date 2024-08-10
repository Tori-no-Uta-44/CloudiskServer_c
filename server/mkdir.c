#include "linked_list.h"
#include "thread_pool.h"
#include "user.h"
#include "mysql.h"
#include "SLogger.h"
#include "mkdir.h"
#include "dataTransfer.h"
//外部变量(userList是在main.c文件中定义的)
extern ListNode * userList;
extern MYSQL *conn;

//读取配置文件
Config read_config(const char*filename){
    Config config={0};
    FILE* file=fopen(filename,"r");
    if(file==NULL){
        perror("unable to open config file\n");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while(fgets(line,sizeof(line),file)){
        char key[MAX_LINE_LENGTH];
        char value[MAX_LINE_LENGTH];

        if(sscanf(line,"%[^=]=%s",key,value)==2){
            if(strcmp(key,"mysql_host")==0){
                strncpy(config.mysql_host,value,sizeof(config.mysql_host));
            }else if (strcmp(key,"mysql_user")==0){
                strncpy(config.mysql_user,value,sizeof(config.mysql_user));
            }else if (strcmp(key,"mysql_passwd")==0){
                strncpy(config.mysql_passwd,value,sizeof(config.mysql_passwd));
            }else if (strcmp(key,"mysql_db")==0){
                strncpy(config.mysql_db,value,sizeof(config.mysql_db));
            }
        }
    }
    fclose(file);
    return config;
}

//连接数据库
MYSQL* connect_to_mysql(const Config* config) {
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }

    if (mysql_real_connect(conn, config->mysql_host,config->mysql_user, config->mysql_passwd, config->mysql_db, 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return NULL;
    }

    printf("MySQL connection successful.\n"); // 添加成功连接打印信息
    return conn;
}

// 执行查询并将结果存放到 Dir_info 结构体中
int fetch_dir_info(MYSQL* conn, const char* username, Dir_info* dir_info) {
    char query[1024];
    // 构造查询语句：获取用户名的用户ID和文件ID
    snprintf(query, sizeof(query),
             "SELECT userId, fileId FROM user WHERE userName='%s'", username);

    // 执行查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "mysql_query() 失败: %s\n", mysql_error(conn));
        return -1;
    }

    // 获取查询结果
    MYSQL_RES* result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        int userId = atoi(row[0]);   // 用户ID
        int fileId = atoi(row[1]);   // 文件ID

        // 使用获取的 fileId 查询路径
        snprintf(query, sizeof(query),
                 "SELECT path FROM virtualFile WHERE fileId=%d", fileId);

        if (mysql_query(conn, query)) {
            fprintf(stderr, "mysql_query() 失败: %s\n", mysql_error(conn));
            mysql_free_result(result);
            return -1;
        }

        // 获取路径查询结果
        MYSQL_RES* path_result = mysql_store_result(conn);
        if (path_result == NULL) {
            fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(conn));
            mysql_free_result(result);
            return -1;
        }

        MYSQL_ROW path_row = mysql_fetch_row(path_result);

        // 填充 Dir_info 结构体
        dir_info->userId = userId;
        dir_info->parentId = fileId; // 假设 parentId 与 fileId 相同
        snprintf(dir_info->path, sizeof(dir_info->path), "%s", path_row ? path_row[0] : "NULL");

        // Optional: 填充 fileName, hashId, fileSize, type 等字段
        // 例如，你可以扩展查询并类似地处理结果

        mysql_free_result(path_result);
    } else {
        // 如果没有找到相关数据
        fprintf(stderr, "没有找到用户数据\n");
        mysql_free_result(result);
        return -1;
    }

    mysql_free_result(result);
    return 0;
}

// 向 globalFile 表中插入一行数据并返回生成的 hashId
int insert_into_global_file(MYSQL* conn) {
    const char* query =
        "INSERT INTO globalFile (hash, linkCount, lastLinkTime) VALUES (NULL, 0, 0)";

    // 执行插入操作
    if (mysql_query(conn, query)) {
        fprintf(stderr, "INSERT 失败: %s\n", mysql_error(conn));
        return -1; // 返回错误码
    }

    // 获取插入后的 auto_increment ID
    long long int last_id = mysql_insert_id(conn);
    if (last_id == 0) {
        fprintf(stderr, "获取插入的 ID 失败\n");
        return -1; // 返回错误码
    }

    return (int)last_id; // 返回新插入的 hashId
}


// 拼接路径的函数
char* concatenatePath(const char *basePath, const char *subPath) {
    size_t baseLen = strlen(basePath);
    size_t subLen = strlen(subPath);
    char *fullPath;

    // 检查 basePath 是否以 '/' 结尾
    int needsSlash = (basePath[baseLen - 1] != '/');

    // 分配内存：考虑 basePath 和 subPath，若需要添加 '/'，再加上 1
    fullPath =(char*)malloc(baseLen + subLen + (needsSlash ? 2 : 1)); // 1 for null terminator

    if (fullPath == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // 复制 basePath
    strcpy(fullPath, basePath);

    // 如果 basePath 没有 '/' 结尾，添加 '/'
    if (needsSlash) {
        fullPath[baseLen] = '/';
        strcpy(fullPath + baseLen + 1, subPath);
    } else {
        strcpy(fullPath + baseLen, subPath);
    }

    return fullPath;
}

// 从数据库获取路径
char* getPathFromDatabase(MYSQL *conn, const char *query) {
    MYSQL_RES *result;
    MYSQL_ROW row;
    char *path = NULL;

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Query failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return NULL;
    }

    if ((row = mysql_fetch_row(result)) != NULL) {
        path = strdup(row[0]);
    }

    mysql_free_result(result);
    return path;
}

// 处理路径的函数
char* parseAndHandlePath(const char *path, MYSQL *conn,char *username) {
    char *fullPath;
    char *dbPath;
    const char *subpath;
    char query[512];

    // 判断路径是否以 "../" 开头
    if (strncmp(path, "../", 3) == 0) {
        subpath = path + 3; // 跳过 "../"
        snprintf(query, sizeof(query),
                 "SELECT path FROM virtualFile WHERE fileId="
                 "(SELECT parentId FROM virtualFile WHERE fileId="
                 "(SELECT fileId FROM user WHERE userName='%s'))",username);
    } else {
        subpath = path;
        snprintf(query, sizeof(query),
                 "SELECT path FROM virtualFile WHERE fileId="
                 "(SELECT fileId FROM user WHERE userName='%s')",username);
    }

    // 从数据库中获取路径
    dbPath = getPathFromDatabase(conn, query);
    if (dbPath == NULL) {
        fprintf(stderr, "Failed to get path from database\n");
        return NULL;
    }

    // 拼接路径
    fullPath = concatenatePath(dbPath, subpath);

    // 释放数据库路径内存
    free(dbPath);

    // 打印拼接后的路径用于调试
    printf("Full path after concatenation: %s\n", fullPath);

    return fullPath;
}

char* extractFinalDir(const char *path) {
    // 如果路径为空，则返回 NULL
    if (path == NULL) {
        return NULL;
    }

    // 检查路径是否以 "../" 开头
    if (strncmp(path, "../", 3) == 0) {
        // 跳过 "../" 部分
        const char *subpath = path + 3;

        // 找到最后一个 '/' 之后的部分，即 dir 部分
        const char *lastSlash = strrchr(subpath, '/');
        if (lastSlash != NULL) {
            // 返回最后一个 '/' 之后的部分
            return strdup(lastSlash + 1);
        } else {
            // 如果没有 '/'，则返回整个 subpath
            return strdup(subpath);
        }
    } else {
        // 直接找到最后一个 '/' 之后的部分，即 dir 部分
        const char *lastSlash = strrchr(path, '/');
        if (lastSlash != NULL) {
            // 返回最后一个 '/' 之后的部分
            return strdup(lastSlash + 1);
        } else {
            // 如果没有 '/'，则返回整个 path
            return strdup(path);
        }
    }
}

// 插入数据到数据库的函数
int insertDirInfo(MYSQL *conn, const Dir_info *dir_info) {
    char query[1024];

    // 构造 INSERT SQL 查询
    snprintf(query, sizeof(query),
             "INSERT INTO virtualFile (parentId, fileName, userId, fileSize, type, path) "
             "VALUES (%d, '%s', %d, %d, '%s', '%s')",
             dir_info->parentId,
             dir_info->fileName,
             dir_info->userId,
             dir_info->fileSize,
             dir_info->type,
             dir_info->path);

    // 执行 SQL 查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "INSERT failed. Error: %s\n", mysql_error(conn));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void mkdirCommand(task_t * task)
{
    printf("Received command: %s\n", task->data);

    Config config=read_config("../conf/server.conf");
    MYSQL* conn=connect_to_mysql(&config);
    if (conn == NULL) {
        exit(0);
    }
    char username[32]={0};
    get_username(username,task->peerfd);

     //填充固定信息
    Dir_info dir_info;
    dir_info.fileSize=0;  //目录大小默认为0
    strcpy(dir_info.type,"Directory");  //类型默认Directory
    char* data=extractFinalDir(task->data);
    strcpy(dir_info.fileName,data);

    //在全局文件表中生成一个hashid,并用dir_info.hashId接收
    //int hashid=insert_into_global_file(conn);
    //dir_info.hashId=hashid;

    fetch_dir_info(conn, username, &dir_info);
    //用dir_info.path接收拼接后的路径
    char* finalpath=parseAndHandlePath(task->data, conn,username);
    strcpy(dir_info.path,finalpath);

    //调试打印
    printf("parentId:%d,filename:%s,userid:%d,filesize:%d,type:%s,path:%s\n",
           dir_info.parentId,dir_info.fileName,dir_info.userId,dir_info.fileSize,dir_info.type,dir_info.path);

    // 插入数据
    if (insertDirInfo(conn, &dir_info) != EXIT_SUCCESS) {
        fprintf(stderr, "Failed to insert data\n");
    } else {
        printf("Data inserted successfully\n");
    }

    // 释放拼接后的路径内存
    free(finalpath);
    mysql_close(conn);

    printf("mkdir command executed.\n");
}

