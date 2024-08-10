#include "thread_pool.h"
#include "fileCheck.h"
#define BUFFER_SIZE 1024
#include "dataTransfer.h"

//文件接收操作
void recvfile(int clientfd,int filefd ,off_t len);
void putsCommand(task_t * task)
{
    //获取任务客户端fd
    int clientfd=task->peerfd;
    printf("execute puts command.\n");
    char filename[BUFFER_SIZE] = {0};
    char src_name[BUFFER_SIZE] = {0};
    //获取任务所需文件名
    strcpy(filename, task->data);
    strcpy(src_name,filename);


    //获取任务所需用户名
    char userName[32] = {0};
    get_username(userName,clientfd);
    printf("Username:%s\n",userName);

    //先校验对端文件是否存在
    off_t flag = 0;
    recvn(clientfd, &flag, sizeof(flag));
    if(flag==-1)
    {
        //客户端上传不存在文件
        printf("No such file or dirction\n");
        addEpollReadfd(task->epfd,clientfd);
        return;
    }


    //接收对端文件的sha_256码
    unsigned char sha_recv[SHA256_DIGEST_LENGTH];
    recv_sha256(clientfd,sha_recv);

    //打开文件的filename改为sha_256;
    char hash_filename[SHA256_DIGEST_LENGTH];
    hash_to_hex_string(sha_recv,hash_filename);
    //打开文件
    int fd = open(hash_filename, O_CREAT|O_RDWR, 0644);
    if(fd < 0) {
        error(1,errno,"open\n"); 
        return;
    }
    //先发送当前文件的大小信息
    struct stat st;
    memset(&st,0,sizeof(st));
    fstat(fd,&st);
    printf("发送当前文件大小信息:%ld\n",st.st_size);
    sendn(clientfd,&st.st_size,sizeof(st.st_size));

    //手动移动文件指针的位置
    lseek(fd,st.st_size,SEEK_SET);

    //接收对端文件长度信息
    off_t len = 0;
    recvn(clientfd, &len, sizeof(len));
    printf("服务器端收到客户端需要发送的长度%ld\n",len);

    //如果收到0
    //表示秒传操作
    if(len==0)
    {
        //秒传操作仅更新虚拟文件列表
        add_virtual(userName,src_name,hash_filename,st.st_size);
        //更新全局文件列表
        updata_link(hash_filename);
        printf("秒传操作\n");
        addEpollReadfd(task->epfd,clientfd);
        close(fd);
        return;
    } 

    //不为0正常操作
    //接收并写入文件
    recvfile(clientfd,fd,len);

    //传输正常结束

    //更新全局文件列表
    add_globFile(hash_filename);
    //更新虚拟文件列表
    add_virtual(userName,src_name,hash_filename,len);
    addEpollReadfd(task->epfd,task->peerfd);

    close(fd);
    printf("recv file is ok\n");
}


void recvfile(int clientfd,int fd,off_t len)    //文件接收实现
{
    int ret=0;
    char buff[1000] = {0};
    //判断是否使用大文件优化技术接收文件
    if (len>1024000)
    {
        //利用零拷贝技术接收文件（splice）  
        //创建一个管道连接内核态两端
        int total=0;
        int recvPipe[2];
        if(pipe(recvPipe)<0)
        {
            error(1,errno,"pipe\n");
        }
        while(total<len)
        {
            ret=splice(clientfd,NULL,recvPipe[1],NULL,4096,SPLICE_F_MORE);
            if(ret==-1)
            {
                error(1,errno,"splice_in\n");
            }

            ret=splice(recvPipe[0],NULL,fd,NULL,ret,SPLICE_F_MORE);
            if(ret==-1)
            {
                error(1,errno,"splice_out\n");
            }
            total+=ret;
        }
        close(recvPipe[0]);
        close(recvPipe[1]);
    }
    else
    {
        //小文件接收
        while(len > 0) {
            if(len < 1000) {
                ret = recvn(clientfd, buff, len);
            } else {
                ret = recvn(clientfd, buff, sizeof(buff));
            }
            if(ret < 0) {
                break;
            }
            ret = write(fd, buff, ret);
            len -= ret;
        }

    }
}
