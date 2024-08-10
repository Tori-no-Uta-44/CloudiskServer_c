#include <stdio.h>
#include <stdlib.h>       // 标准库，包含 exit() 函数
#include <string.h>       // 字符串处理函数
#include <mysql/mysql.h>
#include <unistd.h>
#include <sys/socket.h>
#include "linked_list.h"
#include "thread_pool.h"
#include "user.h"
#include "mysql.h"
#include "SLogger.h"
#include "dataTransfer.h"
//外部变量(userList是在main.c文件中定义的)
extern ListNode * userList;
extern MYSQL *conn;

#define MAX_LINE_LENGTH 256
#define MAX_PATH_LENGTH 1024

typedef struct{
    char mysql_host[64];
    char mysql_user[64];
    char mysql_passwd[64];
    char mysql_db[64];
}Config;

typedef struct{
    int parentId;
    char fileName[32];
    int userId;
    int hashId;
    int fileSize;
    char type[16];
    char path[128];
}Dir_info;

//读取配置文件
Config read_config(const char*filename);
//连接数据库
MYSQL* connect_to_mysql(const Config* config);
// 执行查询并将结果存放到 Dir_info 结构体中
int fetch_dir_info(MYSQL* conn, const char* username, Dir_info* dir_info);
// 向 globalFile 表中插入一行数据并返回生成的 hashId
int insert_into_global_file(MYSQL* conn);
// 拼接路径的函数
char* concatenatePath(const char *basePath, const char *subPath);
// 从数据库获取路径
char* getPathFromDatabase(MYSQL *conn, const char *query);
// 处理路径的函数
char* parseAndHandlePath(const char *path, MYSQL *conn,char *username);
char* extractFinalDir(const char *path);
// 插入数据到数据库的函数
int insertDirInfo(MYSQL *conn, const Dir_info *dir_info);
//执行指令
void mkdirCommand(task_t * task);

