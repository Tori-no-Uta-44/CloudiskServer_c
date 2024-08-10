#include "client.h"
#include "str_util.h"
#include "fileCheck.h"
#include <stdio.h>
#include <unistd.h>
#define BUFFER_SIZE 1024


void recvfile(int clientfd,int fd,off_t len);
void getsCommand(int clientfd,train_t *pt)
{
    char filename[20] = {0};
    strcpy(filename, pt->buff);
    off_t len = 0;


    //先接收校验信息
    off_t flag=0;
    recvn(clientfd,&flag,sizeof(flag));
    if(flag==-1)
    {
        printf("No such file or directory\n");
        return;
    }


    //打开文件
    int fd = open(filename, O_CREAT|O_RDWR, 0644);
    //打开失败处理
    if(fd < 0) {
        perror("open"); return;
    }
    //先发送当前文件的大小信息
    struct stat st;
    memset(&st,0,sizeof(st));
    fstat(fd,&st);
    sendn(clientfd,&st.st_size,sizeof(st.st_size));
    //手动移动文件指针的位置
    lseek(fd,st.st_size,SEEK_SET);


    //接收对端文件长度信息
    recvn(clientfd, &len, sizeof(len));

    //接收文件操作
    recvfile(clientfd,fd,len);


    //文件结束接收操作
    printf("recv file is ok\n");
    close(fd);
//    unsigned char sha_256[SHA256_DIGEST_LENGTH];
//    recv_sha256(clientfd,sha_256);

}




void recvfile(int clientfd,int fd,off_t len)
{
    //接收并写入文件
    char buff[1000] = {0};
    off_t left = len;
    int ret =0;
    //判断是否使用大文件优化技术接收文件
    if (left>1024000)
    {
        //利用零拷贝技术接收文件（splice）  
        //创建一个管道连接内核态两端
        int total=0;
        int recvPipe[2];
        if(pipe(recvPipe)<0)
        {
            error(1,errno,"pipe\n");
        }
        while(total<left)
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
        while(left > 0) {
            if(left < 1000) {
                ret = recvn(clientfd, buff, left);
            } else {
                ret = recvn(clientfd, buff, sizeof(buff));
            }
            if(ret < 0) {
                break;
            }
            ret = write(fd, buff, ret);
            left -= ret;
        }

    }
}
