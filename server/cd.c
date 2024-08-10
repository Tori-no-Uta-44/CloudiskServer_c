/*
 *@author wsr created
 *
 *
 *
 */
#include "db_opration.h"
#include "path_handler.h"
#include "thread_pool.h"
#include "dataTransfer.h"
#include <mysql/mysql.h>
#include <sys/wait.h>

extern MYSQL *conn;

void cdCommand(task_t *task) {
    printf("execute cd command.\n");

    // get user name
    char username[32] = {0};
    get_username(username, task->peerfd);
    printf("user name : %s\n", username);

    // get pwd
    char *path = get_user_pwd(conn, username);
    if (path == NULL)
        error(0, errno, "get_user_pwd");
    printf("user pwd: %s\n", path);
    char *whole_path = get_whole_path(task->data, path);
    if (whole_path == NULL)
        error(0, 0, "get_whole_path");
    // 检查最后一个字符是否是反斜杠
    if (whole_path[strlen(whole_path) - 1] == '/') {
        // 替换最后一个字符为空字符
        whole_path[strlen(whole_path) - 1] = '\0';
    }
    printf("whole file path: %s\n", whole_path);

    int userId = get_user_id(conn, username);
    if (userId == -1)
        error(0, errno, "get_user_id");

    // 从 v表中获取dirID, 并更新u表fId
    // 更新user表
    int ret = upd_dir_in_u(conn, userId, 1, whole_path);
    if (ret == 1) {
        char pwd[128] = {0};
        sprintf(pwd, "pwd: %s", whole_path);
        send(task->peerfd, pwd, strlen(pwd), 0);
    }  

    else {
        send(task->peerfd, "error path, cd failed.", 22, 0);
    }

}

