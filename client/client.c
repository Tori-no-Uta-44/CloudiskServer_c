#include "client.h"
#include "str_util.h"
#include <stdio.h>
#include <unistd.h>

int tcpConnect(const char * ip, unsigned short port)
{
    //1. 创建TCP的客户端套接字
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0) {
        perror("socket");
        return -1;
    }

    //2. 指定服务器的网络地址
    struct sockaddr_in serveraddr;
    //初始化操作,防止内部有脏数据
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;//指定IPv4
    serveraddr.sin_port = htons(port);//指定端口号
    //指定IP地址
    serveraddr.sin_addr.s_addr = inet_addr(ip);

    //3. 发起建立连接的请求
    int ret = connect(clientfd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(ret < 0) {
        perror("connect");
        close(clientfd);
        return -1;
    }
    return clientfd;
}



static int userLogin1(int sockfd, train_t *t);
static int userLogin2(int sockfd, train_t *t);


int userLogin(int sockfd)
{
    int ret;
    train_t t;
    memset(&t, 0, sizeof(t));
    userLogin1(sockfd, &t);
    userLogin2(sockfd, &t);
    return 0;
}


static int userLogin1(int sockfd, train_t* pt)//clientfd，pt指针是传入传出参数
{
    train_t t;
    memset(&t, 0, sizeof(t));
    
    while (1) {
        printf(USER_NAME);//please input a valid user name:\n
        
        char user[20] = { 0 };//初始化user缓冲区
        int ret = read(STDIN_FILENO, user, sizeof(user));//把输入的用户名读到user缓冲区
        user[ret - 1] = '\0';//读取标准输入时将/去掉
        
        //打包发送用户名信息
        t.len = strlen(user);
        t.type = TASK_LOGIN_SECTION1;
        strncpy(t.buff, user, t.len);
        
        ret = sendn(sockfd, &t, 8 + t.len);
        

        //接收服务器消息告知用户名是否合法
        memset(&t, 0, sizeof(t));
        ret = recvn(sockfd, &t.len, 4);
        /* printf("length: %d\n", t.len); */
        ret = recvn(sockfd, &t.type, 4);
        if (t.type == TASK_LOGIN_SECTION1_RESP_ERROR) {
            //无效用户名, 重新输入
            printf("user name not exist.\n");
            continue;
        }
        //读取salt
        ret = recvn(sockfd, t.buff, t.len);//salt由服务器发送
        printf("saltmsg:%s\n",t.buff);
        
        break;
    }
    
    memcpy(pt, &t, sizeof(t));//将t中获取到的salt值存储到pt中
    return 0;
}



static int userLogin2(int sockfd, train_t* pt)//根据salt和密码发送加密密文给服务器
{
    int ret;
    train_t t;
    memset(&t, 0, sizeof(t));
    
    while (1) {
        char* passwd = getpass(PASSWORD);
        char* encrytped = crypt(passwd, pt->buff);//将pt中的salt值和用户密码一起生成加密密文
        printf("encrytped:%s\n",encrytped);

        //将加密密文打包发送给服务器
        t.len = strlen(encrytped);
        t.type = TASK_LOGIN_SECTION2;//发送密文类型
        strncpy(t.buff, encrytped, t.len);
        
        ret = sendn(sockfd, &t, 8 + t.len);

        
        
        //接收服务端响应信息是否登录成功
        memset(&t, 0, sizeof(t));
        ret = recvn(sockfd, &t.len, 4);
        ret = recvn(sockfd, &t.type, 4);
        
        if (t.type == TASK_LOGIN_SECTION2_RESP_ERROR) {
            //密码不正确
            printf("sorry, password is not correct.\n");
            continue;
        }
        else if(t.type == TASK_LOGIN_SECTION2_RESP_OK){
            //ret = recvn(sockfd, t.buff, t.len);
            
            printf("Login Success.\n");
            printf("please input a command.\n");
            break;
        }
    }
    return 0;
}



//其作用：确定接收len字节的数据
int recvn(int sockfd, void * buff, int len)
{
    int left = len;//还剩下多少个字节需要接收
    char * pbuf = buff;
    int ret = -1;
    while(left > 0) {
        ret = recv(sockfd, pbuf, left, 0);
        if(ret == 0) {
            break;
        } else if(ret < 0) {
            perror("recv");
            return -1;
        }

        left -= ret;
        pbuf += ret;
    }
    //当退出while循环时，left的值等于0
    return len - left;
}

//作用: 确定发送len字节的数据
int sendn(int sockfd, const void * buff, int len)
{
    int left = len;
    const char * pbuf = buff;
    int ret = -1;
    while(left > 0) {
        ret = send(sockfd, pbuf, left, 0);
        if(ret < 0) {
            perror("send");
            return -1;
        }

        left -= ret;
        pbuf += ret;
    }
    return len - left;
}

//解析命令
int parseCommand(const char * pinput, int len, train_t * pt)
{
    char * pstrs[10] = {0};
    int cnt = 0;
    splitString(pinput, " ", pstrs, 10, &cnt);
    pt->type = getCommandType(pstrs[0]);
    //暂时限定命令行格式为：
    //1. cmd
    //2. cmd content
    if(cnt > 1) {
        pt->len = strlen(pstrs[1]);
        strncpy(pt->buff, pstrs[1], pt->len);
    }
    return 0;
}

int getCommandType(const char * str)
{
    if(!strcmp(str, "pwd")) 
        return CMD_TYPE_PWD;
    else if(!strcmp(str, "ls"))
        return CMD_TYPE_LS;
    else if(!strcmp(str, "cd"))
        return CMD_TYPE_CD;
    else if(!strcmp(str, "rm"))
        return CMD_TYPE_RM;
    else if(!strcmp(str, "mkdir"))
        return CMD_TYPE_MKDIR;
    else if(!strcmp(str,"rmdir"))
        return CMD_TYPE_RMDIR;
    else if(!strcmp(str, "puts"))
        return CMD_TYPE_PUTS;
    else if(!strcmp(str, "gets"))
        return CMD_TYPE_GETS;
    else
        return CMD_TYPE_NOTCMD;
}

