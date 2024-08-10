#include "dataTransfer.h"
#include "mysql.h"

void get_curPath(char *path,char *userName)
{
    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    MYSQL_RES *res;
    MYSQL_ROW row;


    char query[512];            // 查询字符串
    char result[512];           // 存储结果的字符数组
    size_t result_len = 0;      // 当前字符数组的长度
    // 使用 sprintf 构建 SQL 查询

    snprintf(query, sizeof(query),
             "SELECT virtualFile.path "
             "FROM `user` "
             "JOIN `virtualFile` ON `user`.`fileId` = `virtualFile`.`fileId` "
             "WHERE `user`.`userName` = '%s'", userName);

    // 执行查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT path failed. Error: %s\n", mysql_error(conn));
    }

    //得到结果集
    res = mysql_store_result(conn);
    if (res == NULL) 
    {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
    }    
    // 清空字符数组
    result[0] = '\0';

    // 将结果保存到字符数组
    while ((row = mysql_fetch_row(res)) != NULL) {
        // 确保结果行的数据存在
        if (row[0] != NULL) {
            // 拼接结果到字符数组
            size_t len = snprintf(result + result_len, sizeof(result) - result_len, "%s", row[0]);
            if (len >= sizeof(result) - result_len) {
                fprintf(stderr, "Result buffer is too small\n");
                break;
            }
            result_len += len;
        }
    }

    // 输出结果
    strcpy(path, result);
    // 释放结果集
    mysql_free_result(res);

}


off_t exit_file(char *filename,char *userName)
{
    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    MYSQL_RES *res;
    MYSQL_ROW row;


    char query[512];  // 查询字符串
    char path[256];
    char result[512];
    memset(path,0,sizeof(path));
    memset(result,0,sizeof(result));
    //先获取当前用户的工作路径
    get_curPath(path,userName);
    snprintf(result, sizeof(result), "%s/%s", path, filename);
    // 拼接查询字符串
    snprintf(query, sizeof(query),
             "SELECT hash "
             "FROM ( "
             "    SELECT `hashId` "
             "    FROM `user` AS user"
             "    JOIN `virtualFile` AS virtul ON user.`userName` = '%s' "
             "    WHERE path = '%s' "
             ") AS sub "
             "JOIN `globalFile` AS globle ON sub.`hashId` = globle.`hashId`",
             userName,result
            );
    // 执行查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
    }

    // 获取结果集
    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
    }

    // 清空结果数组
    memset(result, 0, sizeof(result));

    // 输出结果到字符数组
    if ((row = mysql_fetch_row(res)) != NULL) {
        snprintf(result, sizeof(result), "%s", row[0]);
    } else {
        snprintf(result, sizeof(result), "No results found");
        printf("No such file or direction\n");
        mysql_free_result(res);
        return -1;
    }

    strcpy(filename, result);
    // 释放结果集
    mysql_free_result(res);

    return 0;
}

// 函数：插入数据
void insertData(MYSQL *conn, const char *query) {
    // 执行 SQL 语句
    if (mysql_query(conn, query)) {
        fprintf(stderr, "INSERT failed. Error: %s\n", mysql_error(conn));
    } else {
        printf("Data inserted successfully.\n");
    }
}
void add_virtual(char *userName,char *filename,char *hash_name,off_t fileSize)
{
    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    char path[512];
    char resultPath[512];
    int parentId=0;
    int userId=0;
    int hashId=0;
    get_fileMess(userName,path,&parentId,&userId);
    get_hashId(hash_name,&hashId);
    //拼接path路径
    snprintf(resultPath,sizeof(resultPath),"%s/%s",path,filename);
    printf("查看拼接好的路径：%s\n",resultPath);
    // 要执行的 SQL 插入语句
    char query[1024]; // SQL 查询字符串
    snprintf(query, sizeof(query),
             "INSERT INTO `virtualFile`"
             "(`parentId`, `fileName`, `userId`, `hashId`,`fileSize`, type, path) "
             "VALUES "
             "(%d, '%s', %d, %d, %ld, 'File', '%s')",
             parentId, filename, userId, hashId, fileSize, resultPath);
    // 插入数据
    insertData(conn, query);

}
void get_hashId(char *hash_name,int *hashId)
{
    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    MYSQL_RES *res;
    MYSQL_ROW row;


    char query[1024];//SQL查询字符串
    snprintf(query,sizeof(query),"select hashId from `globalFile` where hash='%s'",hash_name);

    // 执行查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "get_fileMess. Error: %s\n", mysql_error(conn));
        return;
    }

    // 获取结果集
    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return;
    }

    // 打印结果集中的每一行
    while ((row = mysql_fetch_row(res))) {
        *hashId=atoi(row[0]);
        printf("看看哈希Id：hashId:%d\n",*hashId);

    }


    //释放结果集
    mysql_free_result(res);

}
void get_fileMess(char *userName,char *path,int *parentId,int *userId)
{
    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    printf("get_fileMess, username: %s\n", userName);
    char query[1024];//SQL查询字符串
    snprintf(query,sizeof(query),
             "select b.* from "
             "`user` as a join `virtualFile` as b "
             "on "
             "a.`fileId`=b.`fileId` "
             "where "
             "a.`userName`='%s'"
             ,userName);

    printf("执行的查询命令：%s\n",query);
    // 执行查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "get_fileMess. Error: %s\n", mysql_error(conn));
        return;
    }

    // 获取结果集
    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return;
    }


    // 打印结果集中的每一行
    while ((row = mysql_fetch_row(res))) {
        *parentId=atoi(row[0]);
        *userId=atoi(row[3]);
        strcpy(path,row[7]);
        printf("看看这些字段行不行：parentId:%d,userId:%d,path:%s\n",*parentId,*userId,path);

    }

    // 释放结果集
    mysql_free_result(res);
}

void get_username(char *userName,int clientfd)
{
    extern ListNode * userList;
    ListNode *current = userList;
    while (current != NULL) {
        user_info_t *user = (user_info_t *)current->val;
        if(user->sockfd==clientfd)
        {
            strcpy(userName,user->name);
        }
        current = current->next;
    }
}
void updata_link(char *hash_name)
{
    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    // 更新数据库
    char update_query[1024];
    snprintf(update_query,sizeof(update_query),
             "update `globalFile` set `linkCount`=`linkCount`+1, lastLinkTime = NULL where hash='%s'",hash_name); 
    printf("执行语句:%s\n",update_query);
    if (mysql_query(conn, update_query)) {
        fprintf(stderr, "UPDATE failed. Error: %s\n", mysql_error(conn));
    }

    // 检查更新操作是否成功
    if (mysql_affected_rows(conn) > 0) {
        printf("Update successful.\n");
    } else {
        printf("No rows affected.\n");
    }
}
void add_globFile(char *hash_name)
{

    extern MYSQL *conn; //外部变量，直接获取主线程的数据库连接信息
    // 要执行的 SQL 插入语句
    char query[1024]; // SQL 查询字符串

    snprintf(query, sizeof(query),
             "insert into `globalFile` (hash,`linkCount`) values ('%s',1)",hash_name);
    // 插入数据
    insertData(conn, query);
}
