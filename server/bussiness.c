#include "linked_list.h"
#include "thread_pool.h"
#include "user.h"
#include "mysql.h"
#include "SLogger.h"
#include "timeWheel.h"
#include "mkdir.h"
//外部变量(userList是在main.c文件中定义的)
extern ListNode * userList;
extern MYSQL *conn;

//查询交互用户的信息
static user_info_t * findUser(int sockfd)
{
    user_info_t * user = NULL;
    ListNode * p = userList;
    //查找用户节点
    while(p != NULL) {
        if(sockfd == ((user_info_t*)p->val)->sockfd) {
            printf("----------found user.\n");
            user = (user_info_t*)p->val;
            break;
        }
        p = p->next;                                                                           
    }       
    return user;
} 
//主线程调用:处理客户端发过来的消息
void handleMessage(int sockfd, int epfd, task_queue_t * que)
{
    //消息格式：cmd content
    //1.1 获取消息长度
    user_info_t *user = findUser(sockfd);
    if(user->recvCnt==0)//说明不是上传或下载的数据
    {
        int length = -1;
        int ret = recvn(sockfd, &length, sizeof(length));
        if(ret == 0) {
            printf("\nconn %d is closed.\n", sockfd);
            delEpollReadfd(epfd, sockfd);
            close(sockfd);
            deleteNode2(&userList, sockfd);//删除用户信息
            return;
        }
        printf("\n\nrecv length: %d\n", length);

        //1.2 获取消息类型
        int cmdType = -1;
        ret = recvn(sockfd, &cmdType, sizeof(cmdType));
        if(ret == 0) {
            printf("\nconn %d is closed.\n", sockfd);
            delEpollReadfd(epfd, sockfd);
            close(sockfd);
            deleteNode2(&userList, sockfd);//删除用户信息
            return;
        }
        printf("recv cmd type: %d\n", cmdType);

        task_t *ptask = (task_t*)calloc(1, sizeof(task_t));
        ptask->peerfd = sockfd;
        ptask->epfd = epfd;
        ptask->type= cmdType;
        if(length > 0) {
            //1.3 获取消息内容
            ret = recvn(sockfd, ptask->data, length);
            if(ret > 0) {
                //往线程池中添加任务
                if((ptask->type == CMD_TYPE_PUTS)||(ptask->type == CMD_TYPE_GETS)) {
                    //是上传文件任务，就暂时先从epoll中删除监听
                    delEpollReadfd(epfd, sockfd);
                }
                taskEnque(que, ptask);
                wheel->updateUserTimeout(wheel, ptask->peerfd, 45);
            }
        } else if(length == 0){
            taskEnque(que, ptask);
        }
    }
    else //说明需要读取的是文件数据
    {
        task_t *ptask = (task_t*)calloc(1, sizeof(task_t));
        ptask->peerfd = sockfd;
        ptask->epfd = epfd;
        ptask->type= CMD_TYPE_RECV;
        delEpollReadfd(epfd, sockfd);
        taskEnque(que,ptask);
        return ;
    }
}

//写事件
void handle_WriteMessage(int sockfd,int epfd ,task_queue_t *que)
{
        task_t *ptask = (task_t*)calloc(1, sizeof(task_t));
        ptask->peerfd = sockfd;
        ptask->epfd = epfd;
        ptask->type= CMD_TYPE_SEND;
        delEpollReadfd(epfd, sockfd);
        taskEnque(que,ptask);
    return;
}

//注意：此函数可以根据实际的业务逻辑，进行相应的扩展
//子线程调用
void doTask(task_t * task)
{
    assert(task);
    switch(task->type) {
    case CMD_TYPE_PWD:  
        pwdCommand(task);
        logInfo(conn, "pwd");
        break;
    case CMD_TYPE_CD:
        cdCommand(task);
        logInfo(conn, "cd");
        break;
    case CMD_TYPE_LS:
        lsCommand(task);
        logInfo(conn, "ls");
        break;
    case CMD_TYPE_RM:
        rmCommand(task);
        logInfo(conn, "rm");
        break;
    case CMD_TYPE_MKDIR:
        mkdirCommand(task);
        logInfo(conn, "mkdir");
        break;
    case CMD_TYPE_RMDIR:
        rmdirCommand(task);
        logInfo(conn, "rmdir");
        break;
    case CMD_TYPE_NOTCMD:
        notCommand(task);
        logInfo(conn, "notcmd");
        break;
    case CMD_TYPE_PUTS:
        putsCommand(task);
        logInfo(conn, "puts");
        break;
    case CMD_TYPE_GETS:
        getsCommand(task);
        logInfo(conn, "gets");
        break;
    case CMD_TYPE_RECV:
        recvCommand(task);
        logInfo(conn, "recv");
        break;
    case CMD_TYPE_SEND:
        sendCommand(task);
        logInfo(conn, "send");
        break;
    case TASK_LOGIN_SECTION1:
        userLoginCheck1(task);
        logInfo(conn, "section1");
        break;
    case TASK_LOGIN_SECTION2:
        userLoginCheck2(task);
        logInfo(conn, "section2");
        break;
    }
}

//每一个具体任务的执行，交给一个成员来实现
/* void cdCommand(task_t *task) */
/* { */
    /* return; */
/* } */

//void mkdirCommand(task_t *task)
//{
//    return;
//}

void userLoginCheck1(task_t * task) {
    printf("userLoginCheck1.\n");
    ListNode * pNode = userList;
    while(pNode != NULL) {
        user_info_t * user = (user_info_t *)pNode->val;
        if(user->sockfd == task->peerfd) {
            //拷贝用户名
            strcpy(user->name, task->data);
            loginCheck1(user);
            return;
        }
        pNode = pNode->next;
    }
}

void userLoginCheck2(task_t * task) {
    printf("userLoginCheck2.\n");
    ListNode * pNode = userList;
    while(pNode != NULL) {
        user_info_t * user = (user_info_t *)pNode->val;
        if(user->sockfd == task->peerfd) {
            //拷贝加密密文
            loginCheck2(user, task->data);
            return;
        }
        pNode = pNode->next;
    }
}

