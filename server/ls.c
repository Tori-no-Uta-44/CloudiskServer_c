/*
 *@author lwh created
 *
 *
 *
 */
#include "thread_pool.h"
#include <mysql/mysql.h>
#include <string.h>       // 字符串处理函数
#include <mysql/mysql.h>
#include <unistd.h>
#include <sys/socket.h>
#include "mkdir.h"  // 需要读取配置文件和连接数据库函数
#include "dataTransfer.h"
#define MAX_LINE_LENGTH 256
#define MAX_FILE_INFO 100 // 最大存储文件信息的数量
#define BUFFER_SIZE 2048


// 定义结构体
typedef struct {
    char filename[256];
    char type[64];
} FileInfo;



// 执行查询并将结果存储到 FileInfo 数组中
int fetch_file_info(MYSQL* conn, const char* query, FileInfo* file_infos, int max_files) {
    if (mysql_query(conn, query)) {
        fprintf(stderr, "QUERY ERROR: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "STORE RESULT ERROR: %s\n", mysql_error(conn));
        return -1;
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    int file_count = 0;

    while ((row = mysql_fetch_row(result)) && file_count < max_files) {
        strncpy(file_infos[file_count].filename, row[0] ? row[0] : "NULL", sizeof(file_infos[file_count].filename));
        strncpy(file_infos[file_count].type, row[1] ? row[1] : "NULL", sizeof(file_infos[file_count].type));
        file_count++;
    }

    mysql_free_result(result);
    return file_count; // 返回实际存储的文件信息数量
}

// 发送文件信息到客户端，并根据文件类型设置颜色
void send_file_info_to_client(task_t* task, const FileInfo* file_info, int count) {
    for (int i = 0; i < count; i++) {
        char message[BUFFER_SIZE];
        const char* color_code = strcmp(file_info[i].type, "Directory") == 0 ? "\033[34m" : ""; // 蓝色
        const char* reset_code = "\033[0m"; // 重置颜色

        snprintf(message, sizeof(message), "%s%s%s  ",
                 color_code,
                 file_info[i].filename,
                 reset_code);

        //调试打印
        //printf("%s", message);

        // 将信息发送到客户端
        if (send(task->peerfd, message, strlen(message),0) < 0) {
            perror("send() failed");
            break;
        }
    }
    printf("\n"); // 打印换行符以结束行
}

void lsCommand(task_t * task)
{
    Config config=read_config("../conf/server.conf");
    MYSQL* conn=connect_to_mysql(&config);
    if (conn == NULL) {
        exit(EXIT_FAILURE);
    }
    //sql操作
    //获取用户名
    // 假设用户名已经获得
    char username[32]={0}; // 这里可以替换为实际获取到的用户名
    get_username(username,task->peerfd);
    //动态插入用户名
    char query[1024]={0};
    snprintf(query, sizeof(query),
             "SELECT fileName, type "
             "FROM virtualFile "
             "WHERE parentId = ("
             "    SELECT fileId "
             "    FROM user "
             "    WHERE userName='%s'"
             ")",
             username);

    FileInfo file_infos[MAX_FILE_INFO];   //结构体数组存储文件数据

    //获取文件数量
    int file_count = fetch_file_info(conn, query, file_infos, MAX_FILE_INFO);

    //发送给客户端
    if (file_count >= 0) {
        send_file_info_to_client(task, file_infos, file_count);
    }

    mysql_close(conn);

}

