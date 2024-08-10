#include <func.h>
#include <mysql/mysql.h>
#include "user.h"
#include "linked_list.h"

void get_curPath(char *path,char *userName);
void add_virtual(char *userName,char *filename,char *hash_name,off_t fileSize);
off_t exit_file(char *filename,char *userName);
void get_fileMess(char *userName,char *path,int *parentId,int *userId);
void insertData(MYSQL *conn, const char *query);
//获取用户名信息
void get_username(char *userName,int clientfd);
void get_hashId(char *hash_name,int *hashId);
void updata_link(char *hash_name);
void add_globFile(char *hash_name);
