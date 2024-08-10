#include "client.h"
#include "str_util.h"
#include "fileCheck.h"
#include <stdio.h>
#include <unistd.h>
#define  BUFFER_SIZE 1024
void sendFile(int clientfd,int filefd ,off_t len);
void putsCommand(int sockfd, train_t * pt)
{

    char filename[20] = {0};
    strcpy(filename, pt->buff);

    //打开文件,发送校验信息
    int fd = open(filename, O_RDWR);
    if(fd < 0) {
        off_t fileError=-1;
        sendn(sockfd, &fileError, sizeof(fileError));
        perror("open"); 
        return;
    }else
    {
      off_t fileError=1;
      sendn(sockfd, &fileError, sizeof(fileError));
    }



    //计算文件的sha_256码信息
    //calculate_sha();
    //发送文件的sha_256码给对端
    //sendsha256();
    send_sha256(sockfd,filename);

    //等待对端发送的存在的文件大小信息
    off_t offSize = 0;
    recvn(sockfd,&offSize,sizeof(offSize));
    printf("接收到服务器已存在文件的大小信息%ld\n",offSize);


    //获取文件大小
    struct stat st;
    memset(&st, 0, sizeof(st));
    fstat(fd, &st);

    //需要发送的文件大小需要改变
    st.st_size -= offSize;

    //如果结果为0提示为妙传操作
    //sendn(sockfd, &st.st_size, sizeof(st.st_size));
    //printf("上传完成");return;
    if(st.st_size==0)
    {
    sendn(sockfd, &st.st_size, sizeof(st.st_size));
    printf("秒传操作\n");
    return;
    }

    //不为0正常操作
    lseek(fd,offSize,SEEK_SET);
    //发送文件大小
    printf("计算出客户端需要上传的文件大小%ld\n",st.st_size);
    sendn(sockfd, &st.st_size, sizeof(st.st_size));

    //执行发送文件操作
    sendFile(sockfd,fd,st.st_size);

    //发送完成
    printf("file send over.\n");

    //正常上传结束

    close(fd);
    sleep(1);

}


void sendFile(int sockfd,int fd ,off_t len)
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
            ret=splice(sendPipe[0],NULL,sockfd,NULL,ret,SPLICE_F_MORE);
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
        ret = 0;
        //发送内容
        while(cur < len) {
            memset(buff, 0, sizeof(buff));
            ret = read(fd, buff, sizeof(buff));
            if(ret == 0) {
                break;
            }
            ret = sendn(sockfd, buff, ret);
            cur +=  ret;
        }
    }
}
