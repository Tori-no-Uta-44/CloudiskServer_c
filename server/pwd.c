
/*
 *@author gsk created
 *
 *
 *
 */


#include "thread_pool.h"
#include "dataTransfer.h"

extern MYSQL *conn;



void pwdCommand(task_t * task){


    char username[32] = {0};
    get_username(username, task->peerfd);


    char path[64] = {0};
    get_curPath(path, username);

    char buff[64] = {0};
    strcpy(buff, path);
    sendn(task->peerfd, buff, strlen(buff));
}
