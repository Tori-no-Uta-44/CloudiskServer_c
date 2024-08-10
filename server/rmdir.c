/*
 *@author wsr created
 *
 *
 *
 */
#include "config.h"
#include "db_opration.h"
#include "path_handler.h"
#include "thread_pool.h"
#include "dataTransfer.h"
#include <sys/wait.h>
#include <stdbool.h>

int rmdir_command(MYSQL *pconn, const char *path, int userId);

void rmdirCommand(task_t * task)
{
    printf("execute rmdir command.\n");

    // connect to mysql
    extern MYSQL *conn;

    // get user name
    char username[32] = {0};
    get_username(username, task->peerfd);
    printf("user name : %s\n", username);

    // get pwd
    char *path = get_user_pwd(conn, username);
    if (path == NULL)
        error(0, errno, "get_user_pwd");
    /* printf("user pwd: %s\n", path); */
    char *whole_path = get_whole_path(task->data, path);
    if (whole_path == NULL)
        error(0, 0, "get_whole_path");
    printf("whole file path: %s\n", whole_path);

    int userId = get_user_id(conn, username);
    if (userId == -1)
        error(0, errno, "get_user_id");

    int num = rmdir_command(conn, whole_path, userId);
    
    char buf[128] = {0};
    if (num == -1) {
        send(task->peerfd, "del dir failed.", 15, 0);
    } else {
        sprintf(buf, "del %d files successfully.", num);
        send(task->peerfd, buf, strlen(buf), 0);
    }
}

int rmdir_command(MYSQL *pconn, const char *path, int userId) {
    char *whole_path = calloc(512, 1);
    strcpy(whole_path, path);

    file_t file_type = Directory;

    // 执行sql语句删除v表记录
    int dirId = del_dir_v(pconn, file_type, userId, whole_path);
    if (dirId == -1) {
        perror("del_dir_v");
        return -1;
    }
    printf("dirId = %d\n", dirId);

    // 获取该目录中的信息
    MYSQL_RES *pres = get_dir_contents(pconn, dirId);
    if (pres == NULL) {
        printf("get_dir_contents return NULL.\n");
        free(whole_path);
        return 0;
    }

    int res_fid = 0;
    char res_type[10] = {0};
    char res_path[512] = {0};
    int num = 0;
        
    MYSQL_ROW row = NULL;
    bool is_content = false;
    while ((row = mysql_fetch_row(pres)) != NULL) {
        is_content = true;
        res_fid = atoi(row[0]);
        strcpy(res_type, row[1]);
        strcpy(res_path, row[2]);

        if (strcmp(res_type, "File") == 0) {
            // 1. 删除该目录下的所有文件
            rm_command(pconn, res_path, userId);
            num ++;
        } else if (strcmp(res_type, "Directory") == 0) {
            // 2. 递归删除该目录下的所有目录
            num +=  rmdir_command(pconn, res_path, userId);
        }
    }
    if (!is_content)
        num = 1;
    
    free_result(pres);
    free(whole_path);

    return num;
}

