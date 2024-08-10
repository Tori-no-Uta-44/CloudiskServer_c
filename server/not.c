/*
 *@author lqy created
 *
 *
 *
 */

#include "thread_pool.h"
#include <func.h>


void notCommand(task_t* task)
{
    printf("execute not command.\n");
    
    char cmd[100];
    strcpy(cmd , task -> data);
    char mes = {"not command"};
    
    char buff[20];
    sprintf(buff, "%s %s", cmd , mes);
    
    send(task -> peerfd, buff, sizeof(buff), 0);

    close(task -> peerfd);
}