#include "thread_pool.h"
#include "fileCheck.h"
#include "dataTransfer.h"
#define  BUFFER_SIZE 1024

void sendFile(int taskfd,int fd ,off_t len);

void getsCommand(task_t * task)
{
    //获取任务客户端fd
    int taskfd=task->peerfd;
    printf("execute gets command.\n");
    char filename[BUFFER_SIZE] = {0};
    //获取任务所需文件名
    strcpy(filename, task->data);
   
    //获取任务用户名
    char userName[32];
    get_username(userName,taskfd);
    printf("Username:%s\n",userName);

    //查找服务器中是否存在目标文件
    //返回值 0：存在 -1：不存在
    //发送验证信息
    off_t flag=exit_file(filename,userName);
    if(flag)
    {
    sendn(taskfd,&flag,sizeof(flag));
    addEpollReadfd(task->epfd,taskfd);
    return;
    }

    printf("flag:%ld\nsha_256:%s\n",flag,filename);
    
    sendn(taskfd,&flag,sizeof(flag));

    //先接收对端文件的大小
    off_t offSize = 0;
    recvn(taskfd,&offSize,sizeof(offSize));
    printf("接收到对端文件的大小信息：%ld\n",offSize);

    //打开目标文件
    int fd = open(filename, O_RDWR);
    off_t fileError=0;
    //打开失败处理
    if(fd < 0) {
        fileError=-1;
        sendn(taskfd, &fileError, sizeof(fileError));
        addEpollReadfd(task->epfd,taskfd);
        perror("open"); return;
    }

    //获取文件大小
    struct stat st;
    memset(&st, 0, sizeof(st));
    fstat(fd, &st);
    //需要发送的文件大小需要改变
    st.st_size -= offSize;
    lseek(fd,offSize,SEEK_SET);

    //发送文件大小
    sendn(taskfd, &st.st_size, sizeof(st.st_size));
    printf("计算出还需要发送的文件大小:%ld\n",st.st_size);
    
    //发送文件
    sendFile(taskfd,fd,st.st_size);
    


    //发送完成
    printf("file send over.\n");
    close(fd);

    //所有操作结束发送校验码信息

    addEpollReadfd(task->epfd,taskfd);
}

void sendFile(int taskfd,int fd , off_t len)
{
    int ret=0;
    //选择是否使用大文件发送技术
    if(len>1024000)
    {
        //利用零拷贝技术发送文件（splice）  
        //创建一个管道连接内核态两端
        int total=0;
        int sendPipe[2];
        if(pipe(sendPipe))
        {
            error(1,errno,"pipe\n");
        }
        while(total<len)
        {
            ret=splice(fd,NULL,sendPipe[1],NULL,4096,SPLICE_F_MORE);
            if(ret==-1)
            {
                error(1,errno,"splice_in\n");
            }
            ret=splice(sendPipe[0],NULL,taskfd,NULL,ret,SPLICE_F_MORE);
            if(ret==-1)
            {
                error(1,errno,"splice_out\n");
            }
            total+=ret;
        }
        close(sendPipe[0]);
        close(sendPipe[1]);
    }
    else
    {
        //小文件发送
        off_t cur = 0;
        char buff[1000] = {0};
        int ret = 0;
        //发送内容
        while(cur < len) {
            memset(buff, 0, sizeof(buff));
            ret = read(fd, buff, sizeof(buff));
            if(ret == 0) {
                break;
            }
            ret = sendn(taskfd, buff, ret);
            cur +=  ret;
        }
    }
}
