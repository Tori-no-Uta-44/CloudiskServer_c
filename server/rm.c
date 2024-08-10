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
#include <sys/wait.h>

int rm_command(MYSQL *pconn, const char *path, int userId);

void rmCommand(task_t * task) {
    printf("execute rm command.\n");

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
    printf("user pwd: %s\n", path);
    char *whole_path = get_whole_path(task->data, path);
    if (whole_path == NULL)
        error(0, 0, "get_whole_path");
    printf("whole file path: %s\n", whole_path);

    int userId = get_user_id(conn, username);
    if (userId == -1)
        error(0, errno, "get_user_id");

    int retval = rm_command(conn, whole_path, userId);
    
    if (retval == 0) {
        send(task->peerfd, "del file successs.", 18, 0);
    } else {
        send(task->peerfd, "del file failed.", 16, 0);
    }

}

int rm_command(MYSQL *pconn, const char *path, int userId) {
    char *whole_path = calloc(512, sizeof(char));
    strcpy(whole_path, path);

    file_t file_type = File;

    // 执行sql语句删除v表记录
    int hashId = del_file_in_v(pconn, file_type, userId, whole_path);
    if (hashId == -1) {
        fprintf(stderr, "del_file_in_v: failed.\n");
        return -1;
    }
    printf("hashId = %d\n", hashId);
    
    // 执行sql语句修改g表记录, 让linkCount - 1
    int lcount = del_file_in_g(pconn, hashId);
    if (lcount == -1) {
        perror("del_file_in_g");
        return -1;
    }
    printf("lcount = %d\n", lcount);

    // 如果 linkCount == 0, 更新g表中的lastLinkTime
    if (lcount == 0) {
        int ret = upd_file_in_g(pconn, hashId);
        if (ret == -1) {
            fprintf(stderr, "upd_file_in_g:failed.\n");
            return -1;
        }
        printf("ret = %d\n", ret);
    }

    // TODO: lastLinkTime间隔一天，彻底删除文件

    free(whole_path);
    
    return 0;
}

int real_del(MYSQL *pconn, int hashId) {

    char *filename = calloc(512, sizeof(char));
    get_hash(pconn, hashId, &filename);

    if (unlink(filename) == -1) {
        fprintf(stderr,
            "Error deleting file %s: %s\n", filename, strerror(errno));
        return 1;
    }

    printf("File %s deleted successfully.\n", filename);
    return 0;
}
