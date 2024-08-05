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

static int userLogin1(int sockfd, train_t *pt)
{
    /* printf("userLogin1.\n"); */
    train_t t;
    memset(&t, 0, sizeof(t));
    while(1) {
        printf(USER_NAME);
        char user[20]= {0};
        int ret = read(STDIN_FILENO, user, sizeof(user));
        user[ret - 1] = '\0';
        t.len = strlen(user);
        t.type = TASK_LOGIN_SECTION1;
        strncpy(t.buff, user, t.len);
        ret = sendn(sockfd, &t, 8 + t.len);
        /* printf("login1 send %d bytes.\n", ret); */

        //接收信息
        memset(&t, 0, sizeof(t));
        ret = recvn(sockfd, &t.len, 4);
        /* printf("length: %d\n", t.len); */
        ret = recvn(sockfd, &t.type, 4);
        if(t.type == TASK_LOGIN_SECTION1_RESP_ERROR) {
            //无效用户名, 重新输入
            printf("user name not exist.\n");
            continue;
        }
        //用户名正确，读取setting
        ret = recvn(sockfd, t.buff, t.len);
        break;
    }
    memcpy(pt, &t, sizeof(t));
    return 0;
}

static int userLogin2(int sockfd, train_t * pt)
{
    /* printf("userLogin2.\n"); */
    int ret;
    train_t t;
    memset(&t, 0, sizeof(t));
    while(1) {
        char * passwd = getpass(PASSWORD);
        /* printf("password: %s\n", passwd); */
        char * encrytped = crypt(passwd, pt->buff);
        /* printf("encrytped: %s\n", encrytped); */
        t.len = strlen(encrytped);
        t.type = TASK_LOGIN_SECTION2;
        strncpy(t.buff, encrytped, t.len);
        ret = sendn(sockfd, &t, 8 + t.len);
        /* printf("userLogin2 send %d bytes.\n", ret); */

        memset(&t, 0, sizeof(t));
        ret = recvn(sockfd, &t.len, 4);
        /* printf("2 length: %d\n", t.len); */
        ret = recvn(sockfd, &t.type, 4);
        if(t.type == TASK_LOGIN_SECTION2_RESP_ERROR) {
            //密码不正确
            printf("sorry, password is not correct.\n");
            continue;
        } else {
            ret = recvn(sockfd, t.buff, t.len);
            printf("Login Success.\n");
            printf("please input a command.\n");
            fprintf(stderr, "%s", t.buff);
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

void putsCommand(int sockfd, train_t * pt)
{
    char filename[20] = {0};
    strcpy(filename, pt->buff);

    //打开文件
    int fd = open(filename, O_RDWR);
    if(fd < 0) {
        perror("open"); return;
    }
    //获取文件大小
    struct stat st;
    memset(&st, 0, sizeof(st));
    fstat(fd, &st);
    printf("file length: %ld\n", st.st_size);
    //发送文件大小
    sendn(sockfd, &st.st_size, sizeof(st.st_size));
    off_t cur = 0;
    char buff[1000] = {0};
    int ret = 0;
    //发送内容
    while(cur < st.st_size) {
        memset(buff, 0, sizeof(buff));
        ret = read(fd, buff, sizeof(buff));
        if(ret == 0) {
            break;
        }
        ret = sendn(sockfd, buff, ret);
        cur +=  ret;
    }
    //发送完成
    printf("file send over.\n");
    close(fd);
}