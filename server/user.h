#ifndef __USER_H__
#define __USER_H__

typedef enum {
    STATUS_LOGOFF = 0,
    STATUS_LOGIN,
    STATUS_REGISTER
}LoginStatus;

typedef struct {
    int recvCnt;            //0 表示未读取flag 1 表示已经读取过flag
    int fileWritefd;        //写入本地文件的fd
    int fileReadfd;         //读取本地文件的fd
    int filesize;           //文件的大小
    int hasSendSize;        //已经发送文件的字节数
    int hasRecvSize;        //已经接收文件的字节数
    char sendBuff[4096];
    char recvBuff[4096];
    int sockfd;             //套接字文件描述符
    LoginStatus status;     //登录状态
    char name[20];          //用户名(客户端传递过来的)
    char encrypted[100];    //从/etc/shadow文件中获取的加密密文
    char pwd[128];          //用户当前路径

}user_info_t;

void loginCheck1(user_info_t * user);
void loginCheck2(user_info_t * user, const char * encrypted);

void registerCheck1(user_info_t* user);
void registerCheck2(user_info_t* user, const char* encrypted);

#endif

